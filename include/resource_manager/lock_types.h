#ifndef LOCK_TYPES_H
#define LOCK_TYPES_H

#include <memory>
#include <shared_mutex>

/**
 * @class ReadLock
 * @brief RAII wrapper para lock de leitura compartilhada
 *
 * Garante que o lock é adquirido na construção e liberado na destruição.
 */
template<typename T>
class ReadLock {
public:
    /**
     * @brief Construtor que adquire o lock de leitura
     * @param resource Recurso a ser acessado
     * @param mutex Mutex compartilhado para controle
     */
    ReadLock(std::shared_ptr<T> resource, std::shared_mutex& mutex);

    /**
     * @brief Destrutor que libera o lock
     */
    ~ReadLock() = default;

    // Não copiável, apenas movível
    ReadLock(const ReadLock&) = delete;
    ReadLock& operator=(const ReadLock&) = delete;
    ReadLock(ReadLock&&) = default;
    ReadLock& operator=(ReadLock&&) = default;

    /**
     * @brief Operador de acesso ao recurso
     * @return Referência para o recurso
     */
    T& operator*();

    /**
     * @brief Operador de acesso por ponteiro
     * @return Ponteiro para o recurso
     */
    T* operator->();

private:
    std::shared_ptr<T> resource;                ///< Recurso protegido
    std::shared_lock<std::shared_mutex> lock;   ///< Lock de leitura
};

/**
 * @class WriteLock
 * @brief RAII wrapper para lock de escrita exclusiva
 *
 * Garante acesso exclusivo ao recurso durante o tempo de vida do lock.
 */
template<typename T>
class WriteLock {
public:
    /**
     * @brief Construtor que adquire o lock de escrita
     * @param resource Recurso a ser acessado
     * @param mutex Mutex compartilhado para controle
     */
    WriteLock(std::shared_ptr<T> resource, std::shared_mutex& mutex);

    /**
     * @brief Destrutor que libera o lock
     */
    ~WriteLock() = default;

    // Não copiável, apenas movível
    WriteLock(const WriteLock&) = delete;
    WriteLock& operator=(const WriteLock&) = delete;
    WriteLock(WriteLock&&) = default;
    WriteLock& operator=(WriteLock&&) = default;

    /**
     * @brief Operador de acesso ao recurso
     * @return Referência para o recurso
     */
    T& operator*();

    /**
     * @brief Operador de acesso por ponteiro
     * @return Ponteiro para o recurso
     */
    T* operator->();

private:
    std::shared_ptr<T> resource;                ///< Recurso protegido
    std::unique_lock<std::shared_mutex> lock;   ///< Lock de escrita
};

// Implementações dos templates
template<typename T>
ReadLock<T>::ReadLock(std::shared_ptr<T> resource, std::shared_mutex& mutex)
    : resource(std::move(resource)), lock(mutex) {}

template<typename T>
T& ReadLock<T>::operator*() { return *resource; }

template<typename T>
T* ReadLock<T>::operator->() { return resource.get(); }

template<typename T>
WriteLock<T>::WriteLock(std::shared_ptr<T> resource, std::shared_mutex& mutex)
    : resource(std::move(resource)), lock(mutex) {}

template<typename T>
T& WriteLock<T>::operator*() { return *resource; }

template<typename T>
T* WriteLock<T>::operator->() { return resource.get(); }

#endif
