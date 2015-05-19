#ifndef TOPICLISTREPORT_H
#define TOPICLISTREPORT_H

#define MAX_SUBSCRIBERS         60

class TopicListReport {
public:
    int32_t numberOfTopics;             ///< total number of topics assigned for the network
    int16_t topicList[MAX_SUBSCRIBERS]; ///< simple list of topic IDs subscribed for network

    TopicListReport() { init(); }      ///< Constructor, initializes topicList.
    void init() { numberOfTopics = 0; }

    /** Look up list, whether a topic is set for network messages.
     * @param[in] topicId ID to check whether topic is listed
     */
    bool find(const short topicId) const {
        for (int i = 0; i < numberOfTopics; i++) {
            if (topicId == topicList[i]) {
                return true;
            }
        }
        return false;
    }

    /** Adds a new topic to list, if not already present.
     * @param[in] topicId ID to add to the list
     */
    void add(const short topicId) {
        if (numberOfTopics < MAX_SUBSCRIBERS - 1) {
            if (find(topicId)) {
                return;
            }
            topicList[numberOfTopics] = topicId;
            numberOfTopics++;
        } else {
//            ERROR("No more topics available\n");
        }
    }

    /// returns the size needed for transmission
    int numberOfBytesToSend() { return (sizeof(long) + numberOfTopics * sizeof(topicList[0])); }
};

#endif // TOPICLISTREPORT_H
