#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include <sqlite3.h>

int ExecuteQuery(sqlite3 *handle, const char *const query) {
    sqlite3_stmt *statement_handle;
    int result = sqlite3_prepare_v2(handle, query, strlen(query), &statement_handle, NULL);
    if (result != SQLITE_OK) {
        return result;
    }
    do {
        result = sqlite3_step(statement_handle);
        if (result != SQLITE_ROW && result != SQLITE_DONE) {
            return result;
        }
    } while (result == SQLITE_ROW);
    return SQLITE_OK;
}

int stop = 0;
pthread_mutex_t stop_mtx;
const char *db_path = "foobar.db";

void KeepRunningQuery(const char *const query) {
    sqlite3 *handle = NULL;
    int ret = sqlite3_open(db_path, &handle);
    if (ret != SQLITE_OK) {
        abort();
    }
    ret = ExecuteQuery(handle, "PRAGMA synchronous = OFF");
    if (ret != SQLITE_OK) {
        abort();
    }
    while (1) {
        ret = ExecuteQuery(handle, query);
        if (ret != SQLITE_OK) {
            abort();
        }
        pthread_mutex_lock(&stop_mtx);
        if (stop == 1) {
            break;
        }
        pthread_mutex_unlock(&stop_mtx);
    }
    pthread_mutex_unlock(&stop_mtx);

    sqlite3_close(handle);
}

void *thread1(void *) {
    KeepRunningQuery("INSERT INTO test_table (something, other) VALUES (\"text1\", \"text2\")");
    return NULL;
}

void *thread2(void *) {
    KeepRunningQuery("SELECT something, other FROM test_table");
    return NULL;
}

int main(int argc, char *argv[]) {
    int ret = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    if (ret != SQLITE_OK) {
        return ret;
    }
    printf("db_path: %s\n", db_path);
    sqlite3 *handle = NULL;
    ret = sqlite3_open(db_path, &handle);
    if (ret != SQLITE_OK) {
        printf("Open failed with %d\n", ret);
        return ret;
    }
    ret = ExecuteQuery(handle, "PRAGMA journal_mode = WAL");
    if (ret != SQLITE_OK) {
        printf("Enable WAL failed with %d\n", ret);
        return ret;
    }
    ret = ExecuteQuery(handle, "CREATE TABLE test_table (something TEXT, other TEXT)");
    if (ret != SQLITE_OK) {
        printf("Create TABLE failed with %d\n", ret);
        return ret;
    }
    sqlite3_close(handle);

    pthread_mutex_init(&stop_mtx, NULL);

    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);

    sleep(5);
    pthread_mutex_lock(&stop_mtx);
    stop = 1;
    pthread_mutex_unlock(&stop_mtx);

    return 0;
}