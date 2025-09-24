#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <thread>
#include <memory>
#include "task_queue.h"

/**
 * @class WorkerThread
 * @brief Thread worker que processa tarefas da fila
 *
 * Cada worker thread fica em loop esperando por tarefas na fila
 * e as executa quando disponíveis.
 */
class WorkerThread {
public:
    /**
     * @brief Construtor que inicia a thread worker
     * @param task_queue Fila compartilhada de tarefas
     */
    explicit WorkerThread(std::shared_ptr<TaskQueue> task_queue);

    /**
     * @brief Destrutor que para a thread
     */
    ~WorkerThread();

    /**
     * @brief Para a thread worker
     */
    void stop();

    /**
     * @brief Junta a thread (espera término)
     */
    void join();

private:
    /**
     * @brief Loop principal da thread worker
     */
    void run();

    std::shared_ptr<TaskQueue> task_queue;      ///< Fila compartilhada de tarefas
    std::thread thread;                         ///< Thread associada
    bool running;                               ///< Flag de execução
};

#endif
