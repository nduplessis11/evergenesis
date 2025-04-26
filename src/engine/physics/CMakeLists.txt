target_sources(engine
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS . FILES
            systems/physics_system.ixx
            components/collider.ixx
            components/transform.ixx
            components/velocity.ixx
        PRIVATE
            systems/physics_system.cpp
)