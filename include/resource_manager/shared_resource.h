#ifndef SHARED_RESOURCE_H
#define SHARED_RESOURCE_H

#include <memory>
#include <shared_mutex>
#include "lock_types.h"

/**
 * @class SharedResource
 * @brief Recurso compartilhado com controle de acesso leitura/escrita
 *
 * Encapsula um recurso com suporte a múltiplos leitores ou um único escritor
 * usando shared_mutex.
 */
template<typename T>
class SharedResource {
public:
    /**
     * @brief Construtor com recurso a ser gerenciado
     * @param resource Recurso a ser compartilhado
     */
    explicit SharedResource(std::shared_ptr<T> resource);

    /**
     * @brief Obtém lock de leitura para o recurso
     * @return ReadLock para acesso de leitura
     */
    ReadLock<T> lock_read();

    /**
     * @brief Obtém lock de escrita para o recurso
     * @return WriteLock para acesso de escrita
     */
    WriteLock<T> lock_write();

    /**
     * @brief Acesso direto ao recurso (sem locking - uso interno)
     * @return Ponteiro para o recurso
     */
    std::shared_ptr<T> get();

private:
    std::shared_ptr<T> resource;                ///< Recurso gerenciado
    std::shared_mutex mutex;                    ///< Mutex para controle de acesso
};

// Implementação do template
template<typename T>
SharedResource<T>::SharedResource(std::shared_ptr<T> resource)
    : resource(std::move(resource)) {}

template<typename T>
ReadLock<T> SharedResource<T>::lock_read() {
    return ReadLock<T>(resource, mutex);
}

template<typename T>
WriteLock<T> SharedResource<T>::lock_write() {
    return WriteLock<T>(resource, mutex);
}

template<typename T>
std::shared_ptr<T> SharedResource<T>::get() {
    return resource;
}

#endif
