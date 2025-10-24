#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

template <typename Type>
class Box
{
private:
    Type contents;         // The item stored in the box
    bool empty = true;     // Indicates if the box is empty
    bool finished = false; // Indicates if the agent is done producing
    std::mutex mtx;
    std::condition_variable cv;

public:
    Box() : contents(Type()), empty(true), finished(false) {} // Default constructor

    // Put an item in the box
    void put(const Type &item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until the box is empty to avoid overwriting
        while ( !empty ) cv.wait(lock);
        contents = item;
        empty = false;
        // std::cout << "Box is now filled with: " << item << std::endl;
        cv.notify_all(); // Notify chefs that the box has a new item
    }

    // Get an item from the box
    Type get()
    {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until the box has an item or the agent is finished
        while ( empty && !finished ) cv.wait(lock);

        // If finished and no more items, return an empty Type
        if (empty)
        {
            return Type();
        }

        Type item = contents;
        empty = true;
        // std::cout << "Box is now empty. Item retrieved: " << item << std::endl;
        cv.notify_all(); // Notify the agent that the box is empty
        return item;
    }

    // Signal that no more items will be produced
    void set_finished()
    {
        std::unique_lock<std::mutex> lock(mtx);
        finished = true;
        cv.notify_all(); // Notify all waiting threads
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
            do
            {
                second = std::rand() % 3;
            } while (second == first);

            std::string pair = ingredients[first] + " and " + ingredients[second];
            std::cout << "(" << std::this_thread::get_id() << ") Agent produced: " << pair << " " << i << std::endl;
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
            std::cout << name << "(" << std::this_thread::get_id() << ") waiting for ingredients... " << i << std::endl;
            std::string item = box.get();

            if (item.empty()) // Exit if no more items will be produced
            {
                std::cout << name << "(" << std::this_thread::get_id() << ") exits. No more resources available." << std::endl;
                break;
            }

            if (item.find(has_ingredient) == std::string::npos)
            {
                std::cout << name << "(" << std::this_thread::get_id() << ") makes and eats a sandwich with: " << item << " and " << has_ingredient << std::endl;
            }
            else
            {
                std::cout << name << "(" << std::this_thread::get_id() << ") skips: " << item << " (owns " << has_ingredient << ")" << std::endl;
                box.put(item); // Put back the item if not usable
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));

            i++;
        }
    }
};

int main(int argc, char **argv)
{
    Box<std::string> box;

    // Create the agent
    Agent<std::string> agent(box);

    // Create the chefs with their respective ingredients
    Chef<std::string> bread_chef("bread", box);
    Chef<std::string> peanut_butter_chef("pb", box);
    Chef<std::string> jam_chef("jam", box);

    // Start threads for the agent and chefs
    std::thread agent_thread(agent);
    std::thread bread_chef_thread(bread_chef, "Bread Chef");
    std::thread peanut_butter_chef_thread(peanut_butter_chef, "Peanut Butter Chef");
    std::thread jam_chef_thread(jam_chef, "Jam Chef");

    // Join threads
    agent_thread.join();
    bread_chef_thread.join();
    peanut_butter_chef_thread.join();
    jam_chef_thread.join();

    return 0;
}
