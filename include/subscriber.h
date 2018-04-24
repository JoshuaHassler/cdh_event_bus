// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#ifndef INCLUDE_SUBSCRIBER_H_
#define INCLUDE_SUBSCRIBER_H_

#include <pthread.h>
#include <OctopOS/tentacle.h>

#include <unordered_map>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <queue>
#include <functional>
#include <tuple>

/*! typedef for the function signature of our callback */
typedef std::function<
    void(std::tuple<sem_id_t, shm_object*, generic_t*>&)> callback;

/*!
 * subscriber_manager provides a base class with static maps that can be shared
 * across the different templated child classes
 */
class subscriber_manager : protected tentacle {
 public:
    /*!
     * wait_for_data provides a "server" funtion that waits for updates
     * notifications from octopOS and then adds the data to the data_queue of
     * the different subscribers. Function signature is like this so it can
     * easily be run in it's own thread.
     * @param data Not used
     * @return Will not return. This function loops infinatly.
     */
    static void* wait_for_data(void* data);

 protected:
    //@{
    /*! mutex locks for the different member data v*/
    static std::mutex topic_ids_lock,
           topic_memory_lock,
           registered_callbacks_lock;
    //@}

    /*!
     * Registers CB to topic; CB will be invoked when new data is available
     * on topic
     * @param cb The callback to be registered for the topic.
     * @param topic The name of the topic to register the callback to.
     * @param size The size of the data object. Only used if a new topic must
     * be created.
     * @param sub A reference to the subscriber class invoking the static
     * method.
     * @return true if registerd successfully, otherwise false.
     */
    static bool register_cb(callback cb, std::string topic, uint size,
        subscriber_manager* sub);

    /*!
     * topic_memory is a map which assosciates all the shared memory data with
     * a topic name
     */
    static std::unordered_map<std::string, std::tuple<sem_id_t, shm_object*,
        generic_t*>> topic_memory;

    /*!
     * topic_ids is a map which assosciates the topic name with its unique
     * identifier
     */
    static std::unordered_map<std::string, octopOS_id_t> topic_ids;

    /*!
     * topic_ids is a map which assosciates the topic name with its callbacks
     */
    static std::unordered_map<std::string, std::vector<callback>>
        registered_callbacks;

    /*!
     * constructs subscriber_manager class. passes the message bus key to the
     * parent tentacle.
     * @param shared_queue The message bus to construct a tentacle on.
     */
    explicit subscriber_manager(key_t shared_queue) : tentacle(shared_queue) {}
};

/*!
 * subscriber is used to retrtieve the data published to a topic so it can be
 * used in the client application. This class provides an easy interface for
 * reading off a topic's data_queue. T is the type of data assosciated with the
 * topic.
 */
template <typename T>
class subscriber : public subscriber_manager {
 public:
    /*!
     * Created a new subscriber to a specific topic. Intializes mutex and
     * condition variable for data_queue protection.
     * @param topic_name The Name of the topic to be subscribed to.
     * @param shared_queue The key for the message queue to construct a tentacle
     * on.
     */
    subscriber(std::string topic_name, key_t shared_queue)
        : subscriber_manager(shared_queue),
        topic(topic_name) {
        pthread_mutex_init(&data_queue_lock, NULL);
        pthread_cond_init(&data_queue_condition, NULL);

        /*! The lambda is the callback. It pushes the new data onto the
        data_queue. */
        if (!subscriber::register_cb(
            [this](std::tuple<sem_id_t, shm_object*, generic_t*> &data) {
            sem_id_t sem_id = std::get<0>(data);
            shm_object* rw = std::get<1>(data);

            if (p(sem_id, 3) < 0)
                exit(1);
            if (p(sem_id, 0) < 0)
                exit(1);
            rw->rw_array[0] += 1;
            if (rw->rw_array[0] == 1)
                if (p(sem_id, 2) < 0)
                    exit(1);
            v(sem_id, 0);
            v(sem_id, 3);

            pthread_mutex_lock(&data_queue_lock);
            data_queue.push(*reinterpret_cast<T*>(std::get<2>(data)));
            pthread_mutex_unlock(&data_queue_lock);

            if (p(sem_id, 0) < 0)
                exit(1);
            rw->rw_array[0] -= 1;
            if (rw->rw_array[0] == 0)
                if (v(sem_id, 2) < 0)
                    exit(1);
            v(sem_id, 0);
            pthread_cond_signal(&data_queue_condition);
        }, topic_name, sizeof(T), this))
        throw std::runtime_error("Failed to register topic");
    }

    /*!
     * Is data available to read?
     * @return Whether data is available.
     */
    bool data_available() {
        return !data_queue.empty();
    }

    /*!
     * Reads from the top of the data queue. If there is no data this will block
     * untill signaled from the callback that there is data available.
     * @return The top of the queue. Is type T.
     */
    T get_data() {
        T return_value;
        pthread_mutex_lock(&data_queue_lock);

        while (data_queue.empty())
             pthread_cond_wait(&data_queue_condition, &data_queue_lock);

        return_value = data_queue.front();
        data_queue.pop();

        pthread_mutex_unlock(&data_queue_lock);
        return return_value;
    }

    /*!
     * Frees pthread primatives
     */
    ~subscriber() {
        pthread_mutex_destroy(&data_queue_lock);
        pthread_cond_destroy(&data_queue_condition);
    }

 private:
    /*! The name of the topic subscribed to */
    std::string topic;

    /*! mutex for locking access to data_queue */
    pthread_mutex_t data_queue_lock;

    /*! condition variable for waiting if there is no data in data_queue */
    pthread_cond_t data_queue_condition;

    /*! The main data queue which holds a list of data as recieved by
        subscriber */
    std::queue<T> data_queue;

    /*! unique identifier for subscriber */
    long id;                                                                      // NOLINT
};
#endif  // INCLUDE_SUBSCRIBER_H_
