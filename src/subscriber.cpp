// Copyright 2017 Space HAUC Command and Data Handling
/*!
 * @file
 */
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

#include "../include/subscriber.h"

bool subscriber_manager::register_cb(callback cb, std::string topic, uint size,
    subscriber_manager* sub) {
    registered_callbacks_lock.lock();
    if (registered_callbacks[topic].empty()) {
        long id = getTempId(tentacle::role_t::SUBSCRIBER);                        // NOLINT Must use long
        uint semid;

        sub->write(CREATE_SUB, std::to_string(id) + " " + std::to_string(size) +
            " " + topic);

        std::pair<long, std::string> response = sub->read(id);                    // NOLINT Must use long

        std::istringstream iss(response.second);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>{}};

        if ((semid = semget(std::stoi(tokens[1]), 2, 0600)) < 0) {
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to get r/w sem");
        }

        shm_object* header = (shm_object*)(shared_data + std::stoi(tokens[0]));   // NOLINT Must do C cast

        topic_memory_lock.lock();
        topic_memory[topic] = std::tuple<sem_id_t, shm_object*, generic_t*>(
            semid,
            header,
            (generic_t*)(header + 1));                                            // NOLINT Must do C cast
        topic_memory_lock.unlock();

        topic_ids_lock.lock();
        topic_ids[topic] = std::stoi(tokens[2]);
        topic_ids_lock.unlock();
    }

    registered_callbacks[topic].push_back(cb);
    registered_callbacks_lock.unlock();

    return true;
}

// TODO(JoshuaHassler) implemnt reader writter for callback list
void* subscriber_manager::wait_for_data(void* data) {
    while (true) {
        topic_memory_lock.lock();
        topic_ids_lock.lock();  // I'm a little nervous about deadlock here
        for (auto topic : topic_memory) {
            std::pair<long, std::string> message =                                // NOLINT Must use long
                tentacle::read(topic_ids[topic.first], false);
            if (message.first) {
                registered_callbacks_lock.lock();
                for (auto cb : registered_callbacks["test"]) {
                    std::cout << "Calling Back: " << std::endl;
                    cb(topic.second);
                }

                registered_callbacks_lock.unlock();
            }
        }
        topic_ids_lock.unlock();
        topic_memory_lock.unlock();
    }
    return NULL;
}

std::unordered_map< std::string,
                    std::tuple< sem_id_t,
                                shm_object*,
                                generic_t*
                              >
                  > subscriber_manager::topic_memory =
    std::unordered_map<std::string,
        std::tuple<sem_id_t, shm_object*, generic_t*>>();


std::unordered_map<std::string, octopOS_id_t> subscriber_manager::topic_ids =
  std::unordered_map<std::string, octopOS_id_t>();

std::unordered_map<std::string, std::vector<callback>>
    subscriber_manager::registered_callbacks =
    std::unordered_map<std::string, std::vector<callback> >();

std::mutex subscriber_manager::topic_ids_lock,
           subscriber_manager::topic_memory_lock,
           subscriber_manager::registered_callbacks_lock;
