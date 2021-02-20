#include "Timeline.hpp"

namespace Profiling {

Thread* Timeline::AddThread(const std::string& name) {
    threads.push_back(Thread(name));
    return &threads.back();
}

Json::Value Timeline::ToJson() const {
    Json::Value value;

    Json::Value threadsJson;
    for (const Thread& thread : threads) {
        threadsJson.append(thread.ToJson());
    }

    value["threads"] = threadsJson;

    return value;
}

void Timeline::FromJson(const Json::Value& value) {
    Clear();

    const Json::Value& threadsJson = value["threads"];
    for (Json::ArrayIndex i = 0; i < threadsJson.size(); i++) {
        Thread* thread = AddThread("");
        thread->FromJson(threadsJson[i]);
    }
}

void Timeline::Clear() {
    threads.clear();
}

}
