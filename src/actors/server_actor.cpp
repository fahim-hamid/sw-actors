#include "actors/type_ids.hpp"
#include "actors/server_actor.hpp"

#include "actors/output_actor.hpp"

#include <caf/typed_event_based_actor.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <optional>
#include <utility>
#include <vector>

namespace sw::actors {
namespace {

struct ServerState {
    ServerActor::pointer self = nullptr;
    ServerOptions options;
    std::vector<Sequence> queries;
    std::vector<Sequence> subjects;
    OutputActor writer;

    std::vector<WorkerActor> workers;
    std::vector<WorkerActor> idle_workers;
    std::deque<AlignmentJob> retry_jobs;
    std::map<std::uint64_t, CompletedAlignment> completed;

    std::size_t next_query = 0;
    std::size_t next_subject = 0;
    std::size_t generated_pairs = 0;
    std::size_t active_jobs = 0;

    std::uint64_t next_job_id = 0;
    std::uint64_t next_output_id = 0;

    bool self_compare = false;
    bool started = false;
    bool scheduler_exhausted = false;
    bool finishing = false;

    std::chrono::steady_clock::time_point start_time;

    std::optional<AlignmentJob> next_job();
    void dispatch_available();
    void dispatch(WorkerActor worker, AlignmentJob job);
    void complete(WorkerActor worker, CompletedAlignment alignment);
    void worker_failed(WorkerActor worker, AlignmentJob job, const caf::error& error);
    void remove_worker(WorkerActor worker);
    void flush_completed();
    void finish_if_complete();
    void shutdown();
};

std::optional<AlignmentJob> ServerState::next_job()
{
    if (!retry_jobs.empty()) {
        AlignmentJob job = std::move(retry_jobs.front());
        retry_jobs.pop_front();
        return job;
    }

    if (scheduler_exhausted)
        return std::nullopt;

    if (options.maximum_pairs != 0
        && generated_pairs >= options.maximum_pairs) {
        scheduler_exhausted = true;
        return std::nullopt;
    }

    if (self_compare) {
        while (next_query < queries.size()) {
            if (next_subject < queries.size()) {
                const Sequence& query = queries[next_query];
                const Sequence& subject = queries[next_subject];
                const AlignmentJob job{
                    .id = next_job_id++,
                    .query_index = static_cast<std::uint64_t>(next_query),
                    .subject_index = static_cast<std::uint64_t>(next_subject),
                    .query = caf::cow_string{query.data},
                    .subject = caf::cow_string{subject.data},
                };

                ++next_subject;
                ++generated_pairs;
                return job;
            }

            ++next_query;
            next_subject = next_query + 1;
        }
    } else {
        while (next_query < queries.size()) {
            if (next_subject < subjects.size()) {
                const Sequence& query = queries[next_query];
                const Sequence& subject = subjects[next_subject];
                const AlignmentJob job{
                    .id = next_job_id++,
                    .query_index = static_cast<std::uint64_t>(next_query),
                    .subject_index = static_cast<std::uint64_t>(next_subject),
                    .query = caf::cow_string{query.data},
                    .subject = caf::cow_string{subject.data},
                };

                ++next_subject;
                ++generated_pairs;
                return job;
            }

            ++next_query;
            next_subject = 0;
        }
    }

    scheduler_exhausted = true;
    return std::nullopt;
}

void ServerState::dispatch_available()
{
    if (!started || finishing)
        return;

    const std::size_t dispatch_window =
        std::max<std::size_t>(1, workers.size() * 2);

    while (!idle_workers.empty()
        && (!retry_jobs.empty()
            || next_job_id - next_output_id < dispatch_window)) {
        std::optional<AlignmentJob> job = next_job();
        if (!job)
            break;

        WorkerActor worker = idle_workers.back();
        idle_workers.pop_back();
        dispatch(worker, std::move(*job));
    }

    finish_if_complete();
}

void ServerState::dispatch(WorkerActor worker, AlignmentJob job)
{
    ++active_jobs;

    self->mail(job)
        .request(worker, caf::infinite)
        .then(
            [this, worker](CompletedAlignment alignment) {
                complete(worker, std::move(alignment));
            },
            [this, worker, job = std::move(job)](const caf::error& error) mutable {
                worker_failed(worker, std::move(job), error);
            });
}

void ServerState::complete(
    WorkerActor worker,
    CompletedAlignment alignment)
{
    --active_jobs;
    completed.emplace(alignment.id, std::move(alignment));
    idle_workers.push_back(worker);

    flush_completed();
    dispatch_available();
}

void ServerState::worker_failed(
    WorkerActor worker,
    AlignmentJob job,
    const caf::error& error)
{
    --active_jobs;
    self->println("Worker failed while processing job {}: {}", job.id, error);

    retry_jobs.push_front(std::move(job));
    remove_worker(worker);
    dispatch_available();
}

void ServerState::remove_worker(WorkerActor worker)
{
    std::erase(workers, worker);
    std::erase(idle_workers, worker);
}

void ServerState::flush_completed()
{
    while (true) {
        std::map<std::uint64_t, CompletedAlignment>::iterator iterator =
            completed.find(next_output_id);
        if (iterator == completed.end())
            return;

        CompletedAlignment alignment = std::move(iterator->second);
        completed.erase(iterator);

        const std::size_t query_index =
            static_cast<std::size_t>(alignment.query_index);
        const std::size_t subject_index =
            static_cast<std::size_t>(alignment.subject_index);
        const Sequence& query = queries[query_index];
        const Sequence& subject =
            self_compare ? queries[subject_index] : subjects[subject_index];

        OutputRecord record{
            .query_id = caf::cow_string{query.id},
            .subject_id = caf::cow_string{subject.id},
            .query = caf::cow_string{query.data},
            .alignment = std::move(alignment),
        };

        self->mail(std::move(record)).send(writer);
        ++next_output_id;
    }
}

void ServerState::finish_if_complete()
{
    if (finishing
        || !started
        || !scheduler_exhausted
        || !retry_jobs.empty()
        || active_jobs != 0
        || !completed.empty()) {
        return;
    }

    finishing = true;

    self->mail(FinishOutput{})
        .request(writer, caf::infinite)
        .then(
            [this](bool output_ok) {
                if (!output_ok)
                    self->println("Alignment output failed");

                const std::chrono::steady_clock::time_point stop_time =
                    std::chrono::steady_clock::now();
                const std::chrono::milliseconds duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        stop_time - start_time);

                self->println(
                    "Completed {} alignment(s) in {} ms",
                    generated_pairs,
                    duration.count());
                shutdown();
            },
            [this](const caf::error& error) {
                self->println("Output actor failed: {}", error);
                shutdown();
            });
}

void ServerState::shutdown()
{
    for (const WorkerActor& worker : workers)
        self->mail(Shutdown{}).send(worker);

    self->mail(Shutdown{}).send(writer);
    self->quit();
}

ServerActor::behavior_type server_actor(
    ServerActor::stateful_pointer<ServerState> self,
    ServerOptions options,
    std::vector<Sequence> queries,
    std::vector<Sequence> subjects)
{
    ServerState& state = self->state();
    state.self = self;
    state.options = std::move(options);
    state.queries = std::move(queries);
    state.subjects = std::move(subjects);
    state.self_compare = state.subjects.empty();
    state.next_subject = state.self_compare ? 1 : 0;
    state.start_time = std::chrono::steady_clock::now();
    state.writer =
        spawn_output_actor(self->system(), state.options.output_file);

    if ((state.self_compare && state.queries.size() < 2)
        || (!state.self_compare
            && (state.queries.empty() || state.subjects.empty()))) {
        state.scheduler_exhausted = true;
    }

    return {
        [self](const RegisterWorker& message) {
            ServerState& state = self->state();
            const WorkerActor worker = message.worker;

            if (state.finishing) {
                self->mail(Shutdown{}).send(worker);
                return;
            }

            const WorkerConfig config{
                .scoring = state.options.scoring,
                .maximum_query_length = static_cast<std::uint64_t>(
                    state.options.maximum_query_length),
                .maximum_subject_length = static_cast<std::uint64_t>(
                    state.options.maximum_subject_length),
            };

            self->mail(config)
                .request(worker, caf::infinite)
                .then(
                    [self, worker]() {
                        ServerState& current = self->state();
                        current.workers.push_back(worker);
                        current.idle_workers.push_back(worker);

                        self->monitor(
                            worker,
                            [self, worker](const caf::error&) {
                                self->state().remove_worker(worker);
                            });

                        current.dispatch_available();
                    },
                    [self](const caf::error& error) {
                        self->println("Failed to configure worker: {}", error);
                    });
        },
        [self](StartPipeline) {
            self->state().started = true;
            self->state().dispatch_available();
            self->state().finish_if_complete();
        },
        [self](Shutdown) {
            self->state().shutdown();
        },
    };
}

}

ServerActor spawn_server(
    caf::actor_system& system,
    ServerOptions options,
    std::vector<Sequence> queries,
    std::vector<Sequence> subjects)
{
    return system.spawn(
        server_actor,
        std::move(options),
        std::move(queries),
        std::move(subjects));
}

}
