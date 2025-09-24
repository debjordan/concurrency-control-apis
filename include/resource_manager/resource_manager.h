#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include "shared_resource.h"
#include "lock_types.h"

/**
 * @class ResourceManager
 * @brief Gerenciador de recursos compartilhados com controle de acesso
 *
 * Gerencia múltiplos recursos com suporte a locks de leitura/escrita
 * e prevenção de deadlocks.
 */
template<typename Key, typename Resource>
class ResourceManager {
public:
    /**
     * @brief Construtor padrão
     */
    ResourceManager() = default;

    /**
     * @brief Obtém acesso de leitura a um recurso
     * @param key Chave do recurso
     * @return Lock de leitura para o recurso
     */
    ReadLock<Resource> get_read_access(const Key& key);

    /**
     * @brief Obtém acesso de escrita a um recurso
     * @param key Chave do recurso
     * @return Lock de escrita para o recurso
     */
    WriteLock<Resource> get_write_access(const Key& key);

    /**
     * @brief Adiciona um novo recurso ao gerenciador
     * @param key Chave do recurso
     * @param resource Recurso a ser adicionado
     */
    void add_resource(const Key& key, std::shared_ptr<Resource> resource);

    /**
     * @brief Remove um recurso do gerenciador
     * @param key Chave do recurso a ser removido
     */
    void remove_resource(const Key& key);

    /**
     * @brief Verifica se um recurso existe
     * @param key Chave do recurso
     * @return true se existe, false caso contrário
     */
    bool contains(const Key& key) const;

    /**
     * @brief Retorna número de recursos gerenciados
     * @return Quantidade de recursos
     */
    size_t size() const;

private:
    mutable std::shared_mutex resources_mutex;  ///< Mutex para proteção do mapa
    std::unordered_map<Key, std::shared_ptr<SharedResource<Resource>>> resources; ///< Mapa de recursos
};

// Implementação do template
template<typename Key, typename Resource>
ReadLock<Resource> ResourceManager<Key, Resource>::get_read_access(const Key& key) {
    std::shared_lock lock(resources_mutex);
    auto it = resources.find(key);
    if (it == resources.end()) {
        throw std::runtime_error("Recurso não encontrado: " + std::to_string(key));
    }
    return it->second->lock_read();
}

template<typename Key, typename Resource>
WriteLock<Resource> ResourceManager<Key, Resource>::get_write_access(const Key& key) {
    std::shared_lock lock(resources_mutex);
    auto it = resources.find(key);
    if (it == resources.end()) {
        throw std::runtime_error("Recurso não encontrado: " + std::to_string(key));
    }
    return it->second->lock_write();
}

template<typename Key, typename Resource>
void ResourceManager<Key, Resource>::add_resource(const Key& key, std::shared_ptr<Resource> resource) {
    std::unique_lock lock(resources_mutex);
    resources[key] = std::make_shared<SharedResource<Resource>>(resource);
}

template<typename Key, typename Resource>
void ResourceManager<Key, Resource>::remove_resource(const Key& key) {
    std::unique_lock lock(resources_mutex);
    resources.erase(key);
}

template<typename Key, typename Resource>
bool ResourceManager<Key, Resource>::contains(const Key& key) const {
    std::shared_lock lock(resources_mutex);
    return resources.find(key) != resources.end();
}

template<typename Key, typename Resource>
size_t ResourceManager<Key, Resource>::size() const {
    std::shared_lock lock(resources_mutex);
    return resources.size();
}

#endif
