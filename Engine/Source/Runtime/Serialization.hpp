#ifndef Serialization_hpp
#define Serialization_hpp

#include "FishEngine.hpp"

//#include "Vector3.hpp"
//#include "Quaternion.hpp"
//#include "Matrix4x4.hpp"
//#include "Object.hpp"
//#include "GameObject.hpp"
//#include "Component.hpp"
//#include "Behaviour.hpp"
//#include "Script.hpp"
//#include "Transform.hpp"
//#include "Camera.hpp"
//#include "CameraController.hpp"
//#include "Scene.hpp"
//#include "Light.hpp"
//#include "MeshRenderer.hpp"
//#include "MeshFilter.hpp"
//#include "SphereCollider.hpp"

#include "ReflectClass.hpp"

#include "Texture.hpp"

#if 1

#include "Archive.hpp"
//#include "ReflectEnum.hpp"

namespace FishEngine
{
	class Meta(NonSerializable) Serialization
	{
	public:
		Serialization() = delete;
		~Serialization() = delete;

		void SerializeScene(OutputArchive& archive);

		template<typename T>
		static void Serialize(OutputArchive& archive, const T & v)
		{
			abort();
		}

		template<typename T>
		static void Deserialize(InputArchive& archive, T & v)
		{
			abort();
		}

		/************************************************************************/
		/* std::unique_ptr                                                      */
		/************************************************************************/

		template<typename T>
		static void Serialize(OutputArchive& archive, const std::unique_ptr<T> & v)
		{
			archive << *v;
		}

		template<typename T>
		static void Deserialize(OutputArchive& archive, std::unique_ptr<T> & v)
		{
			if (v == nullptr)
				v = std::make_unique<T>();
			archive >> *v;
		}

		/************************************************************************/
		/* raw pointers(disabled)                                               */
		/************************************************************************/

		template<typename T>
		static void Serialize(OutputArchive& archive, T * v) = delete;

		template<typename T>
		static void Deserialize(OutputArchive& archive, T * v) = delete;

		/************************************************************************/
		/* std::shared_ptr                                                      */
		/************************************************************************/

		template<typename T, typename std::enable_if_t<std::is_base_of<Component, T>::value, int> = 0>
		static void Serialize(OutputArchive& archive, const std::shared_ptr<T> & v)
		{
			archive << v->m_uuid;
		}

		template<typename T>
		static void Deserialize(InputArchive& archive, std::shared_ptr<T> & v) {}

		static void Serialize(OutputArchive& archive, const std::shared_ptr<Texture> & v)
		{
			archive << v->m_uuid;
		}

		/************************************************************************/
		/* std::weak_ptr                                                        */
		/************************************************************************/

		template<typename T>
		static void Serialize(OutputArchive& archive, const std::weak_ptr<T> & v)
		{
		}

		template<typename T>
		static void Deserialize(InputArchive& archive, std::weak_ptr<T> & v) {}

		/************************************************************************/
		/* std::vector                                                          */
		/************************************************************************/

		//! Serialization for std::vectors of arithmetic (but not bool) using binary serialization
		template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, int> = 0>
		static void Serialize(OutputArchive& archive, const std::vector<T> & v)
		{
			archive << v.size();
			archive.SaveBinary(v.data(), v.size() * sizeof(T));
		}

		// std::vectors of arithmetic (but not bool)
		template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, int> = 0>
		static void Deserialize(InputArchive& archive, std::vector<T> & v)
		{
			std::size_t size;
			archive >> size;
			v.resize(size);
			archive.LoadBinary(v.data(), size);
		}

		//! Serialization for non-arithmetic vector types
		template<typename T, typename std::enable_if_t<!std::is_arithmetic<T>::value, int> = 0>
		static void Serialize(OutputArchive& archive, const std::vector<T> & v)
		{
			archive << v.size();
			for (auto && x : v)
				archive << x;
		}

		// non-arithmetic vector types
		template<typename T, typename std::enable_if_t<!std::is_arithmetic<T>::value, int> = 0>
		static void Deserialize(InputArchive& archive, std::vector<T> & v)
		{
			std::size_t size;
			archive >> size;
			v.resize(size);
			for (auto && x : v)
				archive >> x;
		}

		/************************************************************************/
		/* std::list                                                            */
		/************************************************************************/

		template<typename T>
		static void Serialize(OutputArchive& archive, const std::list<T> & v)
		{
			archive << v.size();
			for (auto const & i : v)
				archive << i;
		}

		template<typename T>
		static void Deserialize(InputArchive& archive, std::list<T> & v)
		{
			std::size_t size;
			archive >> size;
			v.resize(size);
			for (auto & i : v)
				archive >> i;
		}
	};
}

#ifndef __REFLECTION_PARSER__
#include "generate/Class_Serialization.hpp"
#endif

#elif 0

#ifndef __REFLECTION_PARSER__
#include "generate/Class_Serialization.hpp"
#endif

#elif 0

#include "Archive.hpp"

namespace FishEngine
{
    class FE_EXPORT Meta(NonSerializable) Serialization
    {
    public:
        Serialization() = delete;
        
        template<typename T>
        static void Serialize(Archive& archive, const std::string& name, T& v)
//        {
//            Debug::LogError("Serialization for %s not implemented", typeid(T).name());
//            abort();
//        }
    };
    
    template<>
    void Serialization::Serialize(Archive& archive, const std::string& name,  const Vector3& v)
    {
        Serialize(archive, "x", v.x);
        Serialize(archive, "y", v.y);
        Serialize(archive, "z", v.z);
    }
    
    template<>
    void Serialization::Serialize(Archive& archive, const std::string& name,  const Quaternion& v)
    {
        Serialize(archive, "x", v.x);
        Serialize(archive, "y", v.y);
        Serialize(archive, "z", v.z);
        Serialize(archive, "w", v.w);
    }
}

#include "generate/Class_Serialization.hpp"

#else

#include <cereal/cereal.hpp>
//#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
//#include <cereal/archives/binary.hpp>
//#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>

CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Object, "Object");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Component, "Component");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Behaviour, "Behaviour");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Camera, "Camera");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Script, "Script");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::CameraController, "CameraController");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Light, "Light");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Renderer, "Renderer");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::MeshRenderer, "MeshRenderer");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::MeshFilter, "MeshFilter");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::Collider, "Collider");
CEREAL_REGISTER_TYPE_WITH_NAME(FishEngine::SphereCollider, "SphereCollider");

//CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClassOne);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Object, FishEngine::Component);

CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Component, FishEngine::Behaviour);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Component, FishEngine::Renderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Component, FishEngine::MeshFilter);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Component, FishEngine::Collider);

CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Behaviour, FishEngine::Camera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Behaviour, FishEngine::Script);
CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Behaviour, FishEngine::Light);

CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Script, FishEngine::CameraController);

CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Renderer, FishEngine::MeshRenderer);

CEREAL_REGISTER_POLYMORPHIC_RELATION(FishEngine::Collider, FishEngine::SphereCollider);

namespace FishEngine
{
    class FE_EXPORT Meta(NonSerializable) Serialization
    {
    public:
        Serialization() = delete;
        
        template<typename Archive, typename T>
        static void Serialize(Archive& archive, T& v)
        {
            Debug::LogError("Serialization for %s not implemented", T::StaticClassName().c_str());
            abort();
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, Vector3& v)
        {
            archive(cereal::make_nvp("x", v.x),
                    cereal::make_nvp("y", v.y),
                    cereal::make_nvp("z", v.z));
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Quaternion& q)
        {
            archive(cereal::make_nvp("x", q.x),
                cereal::make_nvp("y", q.y),
                cereal::make_nvp("z", q.z),
                cereal::make_nvp("w", q.w));
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Object& value)
        {
            archive(cereal::make_nvp("name", value.m_name));
        }

#define CEREAL_BASE_CLASS(T) cereal::make_nvp(#T, cereal::base_class<T>(&value))

        template<typename Archive>
        static void Serialize(Archive& archive, Component& value)
        {
            archive(CEREAL_BASE_CLASS(Object));
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Transform& value)
        {
            archive(
                CEREAL_BASE_CLASS(Component),
                cereal::make_nvp("localPosition", value.m_localPosition),
                cereal::make_nvp("localRotation", value.m_localRotation),
                cereal::make_nvp("localScale", value.m_localScale)
            );
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Behaviour& value)
        {
            archive(
                CEREAL_BASE_CLASS(Component),
                cereal::make_nvp("enabled", value.m_enabled)
            );
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Camera& value)
        {
            archive(
                CEREAL_BASE_CLASS(Behaviour),
                cereal::make_nvp("fieldOfView",     value.m_fieldOfView),
                cereal::make_nvp("orthographic",    value.m_orthographic),
                cereal::make_nvp("aspect",          value.m_aspect),
                cereal::make_nvp("isAspectSet",     value.m_isAspectSet),
                cereal::make_nvp("farClipPlane",    value.m_farClipPlane),
                cereal::make_nvp("nearClipPlane",   value.m_nearClipPlane),
                cereal::make_nvp("orthographic",    value.m_orthographic)
            );
        }

        template<typename Archive>
        static void Serialize(Archive& archive, GameObject& value)
        {
            archive(
                CEREAL_BASE_CLASS(Object),
                cereal::make_nvp("tag", value.m_tag),
                cereal::make_nvp("activeSelf", value.m_activeSelf),
                cereal::make_nvp("layer", value.m_layer),
                cereal::make_nvp("transform", value.m_transform),
                cereal::make_nvp("components", value.m_components)
            );
        }

        template<typename Archive>
        static void Serialize(Archive& archive, Script& value)
        {
            archive(
                CEREAL_BASE_CLASS(Behaviour)
            );
        }

        template<typename Archive>
        static void Serialize(Archive& archive, CameraController& value)
        {
            archive(
                CEREAL_BASE_CLASS(Script)
            );
        }

//        template<typename Archive>
//        static void Serialize(Archive& archive, Scene& value)
//        {
//            archive(
//                value.m_gameObjects
//            );
//        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, Light& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Behaviour),
                    cereal::make_nvp("type", value.m_type)
                    );
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, Renderer& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Component)
                    );
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, MeshRenderer& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Renderer)
                    );
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, MeshFilter& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Component)
                    );
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, Collider& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Component)
                    );
        }
        
        template<typename Archive>
        static void Serialize(Archive& archive, SphereCollider& value)
        {
            archive(
                    CEREAL_BASE_CLASS(Collider)
                    );
        }
#undef CEREAL_BASE_CLASS
    };

#if 0
    // not work....
    // for cereal serialization
    template<typename Archive, typename T>
    static void serialize(Archive& archive, T& value)
    {
        Serialization::Serialize<Archive>(archive, value);
    }
    
#else
    
#define FE_SERIALIZE(T) \
    template<typename Archive> \
    static void serialize(Archive& archive, T& value) \
    { \
        Serialization::Serialize(archive, value); \
    }

    FE_SERIALIZE(Vector3);
    FE_SERIALIZE(Quaternion);

    FE_SERIALIZE(Object);
    FE_SERIALIZE(Component);
    FE_SERIALIZE(Transform);
    FE_SERIALIZE(GameObject);
    FE_SERIALIZE(Camera);
    FE_SERIALIZE(Script);
    FE_SERIALIZE(CameraController);
    //FE_SERIALIZE(Scene);
    FE_SERIALIZE(Light);
    FE_SERIALIZE(Renderer);
    FE_SERIALIZE(MeshRenderer);
    FE_SERIALIZE(MeshFilter);
    FE_SERIALIZE(Collider);
    FE_SERIALIZE(SphereCollider);
#undef FE_SERIALIZE
#endif
}

#endif

#endif //Serialization_hpp
