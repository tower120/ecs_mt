#include "benchmark_utils.hpp"

#include <iostream>
#include <tower120/ecs/world.hpp>

using namespace tower120::ecs;

struct data_x{ std::size_t x;};
struct data_y{ std::size_t y;};
struct data_z{ std::size_t z;};

int main(){
    const std::size_t times = 100;
    const std::size_t count = 100'000;

    {
        const auto t = benchmark(times, [&](){
            std::vector<data_x> xs;
            std::vector<data_y> ys;
            for(std::size_t i = 0; i < count;++i){
                xs.push_back(data_x{i});
                ys.push_back(data_y{i});
            }
        });
        std::cout << "vector emplace_back : " << t.count() << std::endl;
    }

    {
        const auto t = benchmark(times, [&](){
            world world;
            entity_manager entity_manager;
            for(std::size_t i = 0; i < count;++i){
                world.make_entity(data_x{i}, data_y{i});
            }
        });
        std::cout << "world make_entity : " << t.count() << std::endl;
    }

    {
        std::chrono::high_resolution_clock::time_point::duration t = std::chrono::high_resolution_clock::time_point::duration::zero();
        for(std::size_t k = 0 ; k < times;++k){
            world world;
            std::vector<entity> entities;
            for(std::size_t i = 0; i < count;++i){
                entities.push_back(
                    world.make_entity(data_x{i})
                );
            }
            t += measure<std::chrono::high_resolution_clock::time_point::duration>([&](){
                for(std::size_t i = 0; i < count;++i){
                    world.change_components(entities[i], data_y{i});
                }
            });
        }
        std::cout << "world change_components : " << std::chrono::duration_cast<std::chrono::milliseconds>(t).count() << std::endl;

    }

    return 0;
}