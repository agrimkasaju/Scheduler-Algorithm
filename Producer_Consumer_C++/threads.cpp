#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

// --- FIX PART 1: A global mutex for console output ---
std::mutex print_mtx; 

template <typename Type>
class Box
{
private:
    Type contents;         
    bool empty = true;     
    bool finished = false; 
    std::mutex mtx;
    std::condition_variable cv;

public:
    Box() : contents(Type()), empty(true), finished(false) {}

    void put(const Type &item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        while ( !empty ) cv.wait(lock);
        contents = item;
        empty = false;
        cv.notify_all();
    }

    Type get()
    {
        std::unique_lock<std::mutex> lock(mtx);
        while ( empty && !finished ) cv.wait(lock);

        if (empty) return Type();

        Type item = contents;
        empty = true;
        cv.notify_all();
        return item;
    }

    void set_finished()
    {
        std::unique_lock<std::mutex> lock(mtx);
        finished = true;
        cv.notify_all();
    }
};

const int count = 20;

template <typename Type>
class Agent
{
private:
    Box<std::string> &box;
    std::vector<std::string> ingredients = {"bread", "pb", "jam"};

public:
    Agent(Box<std::string> &a_box) : box(a_box) {}

    void operator()()
    {
        std::srand(std::time(nullptr));
        
        int i = 0;
        while (i < count)
        {
            int first = std::rand() % 3;
            int second;
            do { second = std::rand() % 3; } while (second == first);

            std::string pair = ingredients[first] + " and " + ingredients[second];
            
            // --- FIX PART 2: Lock before printing ---
            {
                std::lock_guard<std::mutex> lock(print_mtx);
                std::cout << "(" << std::this_thread::get_id() << ") Agent produced: " << pair << " " << i << std::endl;
            }

            box.put(pair);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            i++;
        }
        box.set_finished();
    }
};

template <typename Type>
class Chef
{
private:
    std::string has_ingredient;
    Box<std::string> &box;

public:
    Chef(const std::string &ingredient, Box<std::string> &a_box) : has_ingredient(ingredient), box(a_box) {}

    void operator()(const std::string &name)
    {
        int i = 0;
        while (true)
        {
            // --- FIX PART 3: Lock before printing ---
            {
                std::lock_guard<std::mutex> lock(print_mtx);
                std::cout << name << "(" << std::this_thread::get_id() << ") waiting for ingredients... " << i << std::endl;
            }

            std::string item = box.get();

            if (item.empty()) 
            {
                std::lock_guard<std::mutex> lock(print_mtx);
                std::cout << name << "(" << std::this_thread::get_id() << ") exits. No more resources available." << std::endl;
                break;
            }

            if (item.find(has_ingredient) == std::string::npos)
            {
                std::lock_guard<std::mutex> lock(print_mtx);
                std::cout << name << "(" << std::this_thread::get_id() << ") makes and eats a sandwich with: " << item << " and " << has_ingredient << std::endl;
            }
            else
            {
                {
                    std::lock_guard<std::mutex> lock(print_mtx);
                    std::cout << name << "(" << std::this_thread::get_id() << ") skips: " << item << " (owns " << has_ingredient << ")" << std::endl;
                }
                box.put(item); 
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i++;
        }
    }
};

int main(int argc, char **argv)
{
    Box<std::string> box;
    Agent<std::string> agent(box);
    Chef<std::string> bread_chef("bread", box);
    Chef<std::string> peanut_butter_chef("pb", box);
    Chef<std::string> jam_chef("jam", box);

    std::thread agent_thread(agent);
    std::thread bread_chef_thread(bread_chef, "Bread Chef");
    std::thread peanut_butter_chef_thread(peanut_butter_chef, "Peanut Butter Chef");
    std::thread jam_chef_thread(jam_chef, "Jam Chef");

    agent_thread.join();
    bread_chef_thread.join();
    peanut_butter_chef_thread.join();
    jam_chef_thread.join();

    return 0;
}
