#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/taskqueue.h"


TEST_CASE("Tasks can be inserted into and retrieved from the queue", "[taskqueue]"){
    biti::TaskQueue taskqueue;
    biti::Task task;
    time_t created = 0;
    task.created = created;
    task.type = biti::TaskType::FILE_SAVE;
    task.arg = "testing";
    
    REQUIRE_NOTHROW(taskqueue.push(task));

    SECTION("inserting an item increases the size of the queue by one"){
        REQUIRE(taskqueue.size() == 1);
    }

    SECTION("Retrieving an item from the queue does not change anything in the item"){
        auto _task = taskqueue.get_task();
        REQUIRE(_task.created == created);
        REQUIRE(_task.arg == "testing");
        REQUIRE(_task.type == biti::TaskType::FILE_SAVE);

        // size of queue goes back to 0
        REQUIRE(taskqueue.size() ==  0);
    }
    
    // TODO how to test for blocking function
}