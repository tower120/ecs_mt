#include "benchmark_utils.hpp"

#include <iostream>
#include <tower120/ecs/world.hpp>
#include <tower120/ecs/query.hpp>

using namespace tower120::ecs;

constexpr static const std::size_t payload_size = 8;

struct data_x{ std::size_t x; std::byte payload[payload_size]{};};
struct data_y{ std::size_t y; std::byte payload[payload_size]{};};
struct data_z{ std::size_t z; std::byte payload[payload_size]{};};


int main(){
    const std::size_t times = 100'000;
    const std::size_t count = 1'000;


    volatile std::size_t vector_sum = 0; (void)vector_sum;
    {
        std::vector<data_x> xs;
        std::vector<data_y> ys;
        for(std::size_t i = 0; i < count;++i){
            xs.push_back(data_x{i});
            ys.push_back(data_y{i});
        }

        std::size_t sum = 0;
        std::chrono::milliseconds t;
        {
            scoped_profiler profiler{t};
            for(std::size_t time_n = 0; time_n < times; ++time_n){
                for(std::size_t i = 0; i < count;++i){
                    sum += xs[i].x;
                    sum += ys[i].y;
                }
            }
        }
        vector_sum = sum;
        std::cout << "vector : " << t.count() << std::endl;
    }


    {
        world world;
        for(std::size_t i = 0; i < count;++i){
            world.make_entity(data_x{i}, data_y{i});
        }

        std::size_t sum = 0;
        std::chrono::milliseconds t;
        {
            scoped_profiler profiler{t};
            for(std::size_t time_n = 0; time_n < times; ++time_n){
                for (const auto& components : query(world).select<data_x, data_y>()){
                    sum += std::get<data_x&>(components).x;
                    sum += std::get<data_y&>(components).y;
                }
            }
        }
        {volatile auto s = sum; (void)s;}
        std::cout << "ecs query select: " << t.count() << std::endl;
        assert(vector_sum == sum);
    }


    {
        world world;
        for(std::size_t i = 0; i < count;++i){
            world.make_entity(data_x{i}, data_y{i});
        }

        std::size_t sum = 0;
        std::chrono::milliseconds t;
        {
            scoped_profiler profiler{t};
            for(std::size_t time_n = 0; time_n < times; ++time_n){
                query(world).foreach([&sum](const data_x& x, const data_y& y){
                    sum += x.x;
                    sum += y.y;
                });
            }
        }
        {volatile auto s = sum; (void)s;}
        std::cout << "ecs query foreach: " << t.count() << std::endl;
        assert(vector_sum == sum);
    }


    {
        world world;
        for(std::size_t i = 0; i < count;++i){
            world.make_entity(data_x{i}, data_y{i});
        }

        std::size_t sum = 0;
        std::chrono::milliseconds t;
        {
            scoped_profiler profiler{t};
            for(std::size_t time_n = 0; time_n < times; ++time_n){
                query(world).contains<data_x, data_y>().foreach_container([&](auto& container) -> bool {
                    const auto& xs = container.template components<data_x>();
                    const auto& ys = container.template components<data_y>();

                    for(std::size_t i = 0; i < count;++i){
                        sum += xs[i].x;
                        sum += ys[i].y;
                    }

                    return true;
                });
            }
        };
        {volatile auto s = sum; (void)s;}
        std::cout << "ecs query foreach container: " << t.count() << std::endl;
        assert(vector_sum == sum);
    }

    {
        world world;
        for(std::size_t i = 0; i < count;++i){
            world.make_entity(data_x{i}, data_y{i});
        }

        std::size_t sum = 0;
        std::chrono::milliseconds t;
        {
            scoped_profiler profiler{t};
            for(std::size_t time_n = 0; time_n < times; ++time_n){
                auto result = query(world).contains<data_x, data_y>().select();
                auto& container = *result.containers().front();

                std::vector<data_x>* xs = &container.components<data_x>();
                std::vector<data_y>* ys = &container.components<data_y>();

                for(std::size_t i = 0; i < count;++i){
                    sum += (*xs)[i].x;
                    sum += (*ys)[i].y;
                }
            }
        }
        {volatile auto s = sum; (void)s;}
        std::cout << "ecs query select container: " << t.count() << std::endl;
        assert(vector_sum == sum);
    }

}