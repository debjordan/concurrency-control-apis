#include "thread_pool/thread_pool.h"

/**
 * @brief Construtor do ThreadPool
 * @param num_threads Número de threads a serem criadas
 */
ThreadPool::ThreadPool(size_t num_threads)
    : task_queue(std::make_shared<TaskQueue>())
    , stop(false) {

    // Cria as threads workers
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(std::make_unique<WorkerThread>(task_queue));
    }
}

/**
 * @brief Destrutor do ThreadPool
 */
ThreadPool::~ThreadPool() {
    // Para a fila de tarefas
    task_queue->stop();

    // Para e junta todas as threads workers
    for (auto& worker : workers) {
        worker->stop();
    }
    for (auto& worker : workers) {
        worker->join();
    }
}

/**
 * @brief Retorna o número de threads no pool
 * @return Número de threads
 */
size_t ThreadPool::size() const {
    return workers.size();
}

/**
 * @brief Verifica se o pool está parado
 * @return true se parado, false caso contrário
 */
bool ThreadPool::stopped() const {
    return stop;
}
