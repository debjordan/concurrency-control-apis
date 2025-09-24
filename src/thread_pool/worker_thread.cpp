#include "thread_pool/worker_thread.h"
#include <iostream>

/**
 * @brief Construtor do WorkerThread
 * @param task_queue Fila compartilhada de tarefas
 */
WorkerThread::WorkerThread(std::shared_ptr<TaskQueue> task_queue)
    : task_queue(std::move(task_queue))
    , running(true) {

    // Inicia a thread com o loop de execução
    thread = std::thread(&WorkerThread::run, this);
}

/**
 * @brief Destrutor do WorkerThread
 */
WorkerThread::~WorkerThread() {
    stop();
    join();
}

/**
 * @brief Loop principal da thread worker
 */
void WorkerThread::run() {
    while (running) {
        TaskQueue::Task task;

        // Tenta obter tarefa da fila
        if (task_queue->pop(task)) {
            try {
                task();  // Executa a tarefa
            } catch (const std::exception& e) {
                std::cerr << "Exceção em WorkerThread: " << e.what() << std::endl;
            }
        } else {
            // Fila parada e vazia, sai do loop
            break;
        }
    }
}

/**
 * @brief Para a thread worker
 */
void WorkerThread::stop() {
    running = false;
}

/**
 * @brief Junta a thread (espera término)
 */
void WorkerThread::join() {
    if (thread.joinable()) {
        thread.join();
    }
}
