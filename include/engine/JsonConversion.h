#pragma once

#include "engine/AnimationComponent.h"
#include "engine/Music.h"
#include "engine/RenderComponent.h"
#include "engine/SoundBuffer.h"
#include "engine/SoundComponent.h"
#include "engine/Sprite.h"
#include "engine/Texture.h"
#include "engine/Transform.h"
#include "engine/UUID.h"
#include "engine/Vector2.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace engine {

class AssetDeserializationManager {
  public:
    static AssetDeserializationManager &instance() {
        static AssetDeserializationManager mgr;
        return mgr;
    }
    AssetDeserializationManager(const AssetDeserializationManager &) = delete;
    AssetDeserializationManager &operator=(const AssetDeserializationManager &) = delete;

    template <typename T>
    std::shared_ptr<T> getSharedResource(const UUID &id, const std::string &filePath) {
        auto map = getMapForType<T>();
        auto it = map->find(id);
        if (it != map->end()) {
            return it->second;
        }
        auto resource = std::make_shared<T>(filePath);
        resource->id = id;
        map->insert({id, resource});
        return resource;
    }

  private:
    AssetDeserializationManager() {}
    std::unordered_map<UUID, std::shared_ptr<Texture>> _textureMap{};
    std::unordered_map<UUID, std::shared_ptr<SoundBuffer>> _soundBufferMap{};
    std::unordered_map<UUID, std::shared_ptr<Music>> _musicMap{};

    template <typename T> std::unordered_map<UUID, std::shared_ptr<T>> *getMapForType() {
        if constexpr (std::is_same_v<T, Texture>) {
            return reinterpret_cast<std::unordered_map<UUID, std::shared_ptr<T>> *>(&_textureMap);
        } else if constexpr (std::is_same_v<T, SoundBuffer>) {
            return reinterpret_cast<std::unordered_map<UUID, std::shared_ptr<T>> *>(
                &_soundBufferMap);
        } else if constexpr (std::is_same_v<T, Music>) {
            return reinterpret_cast<std::unordered_map<UUID, std::shared_ptr<T>> *>(&_musicMap);
        } else {
            static_assert(false, "Unsupported type for AssetDeserializationManager");
        }
    }
};

class ComponentFactory {
  public:
    using Creator = std::unique_ptr<Component> (*)(const json &);

    static std::unique_ptr<Component> create(const std::string &type, const json &data) {
        auto it = creators().find(type);
        if (it != creators().end())
            return it->second(data);
        throw std::runtime_error("Unknown component type: " + type);
    }

    static void registerType(std::string type, Creator creator) {
        creators()[std::move(type)] = creator;
    }

  private:
    static std::unordered_map<std::string, Creator> &creators() {
        static std::unordered_map<std::string, Creator> map;
        return map;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, scale, rotation)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UUID, high, low)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rect, left, top, width, height)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FrameInfo, framesPerSecond, totalFrames, width, height,
                                   verticalOffset, horizontalOffset, horizontalFrameCount,
                                   verticalFrameCount, horizontalPadding, verticalPadding,
                                   positionDelta)

// Texture-SharedPointer serialization
void to_json(json &j, const std::shared_ptr<Texture> &t) {
    j = json{{"id", t->id}, {"filePath", t->getFilePath()}};
}

void from_json(const json &j, std::shared_ptr<Texture> &t) {
    UUID id;
    j.at("id").get_to(id);
    std::string filePath = j.at("filePath").get<std::string>();
    t = AssetDeserializationManager::instance().getSharedResource<Texture>(id, filePath);
}

// SoundBuffer
void to_json(json &j, const std::shared_ptr<SoundBuffer> &s) {
    j = json{{"id", s->id}, {"filePath", s->getFilePath()}};
}

void from_json(const json &j, std::shared_ptr<SoundBuffer> &s) {
    j.at("id").get_to(s->id);
    s->setLoadSoundBuffer(j.at("filePath").get<std::string>());
}

// Music
void to_json(json &j, const std::shared_ptr<Music> &m) {
    j = json{{"id", m->id}, {"filePath", m->getFilePath()}};
}
void from_json(const json &j, std::shared_ptr<Music> &m) {
    j.at("id").get_to(m->id);
    m->setLoadMusic(j.at("filePath").get<std::string>());
}

// AnimationComponent
void to_json(json &j, const std::unique_ptr<AnimationComponent> &a) {
    j = json{{"frameInfo", a->getFrameInfo()}};
}

void from_json(const json &j, std::unique_ptr<AnimationComponent> &a) {
    a->setFrameInfo(j.at("frameInfo").get<FrameInfo>());
}

// RenderComponent
void to_json(json &j, const std::unique_ptr<RenderComponent> &r) {
    j = json{{"layer", r->layer},
             {"zIndex", r->zIndex},
             {"pivot", r->getPivot()},
             {"repeated", r->isRepeated()},
             {"texture", r->getTexture()}};
}
void from_json(const json &j, std::unique_ptr<RenderComponent> &r) {
    r->layer = j.at("layer").get<int>();
    r->zIndex = j.at("zIndex").get<int>();
    r->setPivot(j.at("pivot").get<Vector2>());
    r->setRepeated(j.at("repeated").get<bool>());
    auto texture = AssetDeserializationManager::instance().getSharedResource<Texture>(
        r->getTexture()->id, r->getTexture()->getFilePath());

    r->setTexture(texture);
}

// SoundComponent
void to_json(json &j, const std::unique_ptr<SoundComponent> &s) {
    j = json{{"soundBuffer", s->getSoundBuffer()}};
}

void from_json(const json &j, std::unique_ptr<SoundComponent> &s) {
    auto soundBuffer = AssetDeserializationManager::instance().getSharedResource<SoundBuffer>(
        s->getSoundBuffer()->id, s->getSoundBuffer()->getFilePath());
    s->setSoundBuffer(soundBuffer);
}

//
/*
void to_json(json &j, const std::unique_ptr<Component> &c) { c->serialize(j); }

void from_json(const json &j, std::unique_ptr<Component> &c) {
    c = ComponentFactory::create(j.at("type").get<std::string>(), j);
}
*/

// GameObject
void to_json(json &j, const std::unique_ptr<GameObject> &g) {
    j = json{{"transform", g->localTransform},
             {"enabled", g->enabled},
             //{"components", g->getComponents()},
             {"children", g->getChildren()}};
}
} // namespace engine
