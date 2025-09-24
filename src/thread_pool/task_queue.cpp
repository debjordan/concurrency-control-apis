#include "thread_pool/task_queue.h"

/**
 * @brief Construtor da TaskQueue
 */
TaskQueue::TaskQueue()
    : stop_flag(false) {}

/**
 * @brief Adiciona tarefa à fila
 * @param task Tarefa a ser adicionada
 * @return true se bem-sucedido, false se fila parada
 */
bool TaskQueue::push(Task task) {
    {
        std::unique_lock lock(mutex);
        if (stop_flag) return false;

        queue.push(std::move(task));
    }
    condition.notify_one();  // Notifica uma thread waiting
    return true;
}

/**
 * @brief Remove tarefa da fila (bloqueante)
 * @param task Referência para armazenar a tarefa
 * @return true se obteve tarefa, false se fila parada
 */
bool TaskQueue::pop(Task& task) {
    std::unique_lock lock(mutex);

    // Espera até que haja tarefas ou a fila seja parada
    condition.wait(lock, [this]() {
        return !queue.empty() || stop_flag;
    });

    if (stop_flag && queue.empty()) return false;

    task = std::move(queue.front());
    queue.pop();
    return true;
}

/**
 * @brief Para a fila e notifica todas as threads
 */
void TaskQueue::stop() {
    {
        std::unique_lock lock(mutex);
        stop_flag = true;
    }
    condition.notify_all();  // Notifica todas as threads waiting
}

/**
 * @brief Verifica se a fila está parada
 * @return true se parada, false caso contrário
 */
bool TaskQueue::stopped() const {
    std::unique_lock lock(mutex);
    return stop_flag;
}

/**
 * @brief Retorna tamanho atual da fila
 * @return Número de tarefas na fila
 */
size_t TaskQueue::size() const {
    std::unique_lock lock(mutex);
    return queue.size();
}
