#include <stdlib.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <sqlite3.h>

namespace {

int ExecuteQuery(sqlite3* handle, const std::string& query) {
  sqlite3_stmt* statement_handle;
  int result = sqlite3_prepare_v2(
      handle, query.c_str(), static_cast<int>(query.size()), &statement_handle,
      nullptr);
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


void KeepRunningQuery(const std::atomic<bool>* stop_ptr, const std::string& db_path, const std::string& query) {
  sqlite3* handle = nullptr;
  int ret = sqlite3_open(db_path.c_str(), &handle);
  if (ret != SQLITE_OK) {
    abort();
  }
  ret = ExecuteQuery(handle, "PRAGMA synchronous = OFF");
  if (ret != SQLITE_OK) {
    abort();
  }
  const std::atomic<bool>& stop = *stop_ptr;
  while (!stop) {
    ret = ExecuteQuery(handle,
                       query.c_str());
    if (ret != SQLITE_OK) {
      abort();
    }
  }
  sqlite3_close(handle);
}

}  // namespace

int main(int argc, char* argv[]) {
  int ret = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
  if (ret != SQLITE_OK) {
    return ret;
  }
  std::string tmp_dir = "/tmp/sqlite_test_XXXXXX";
  if (!mkdtemp(tmp_dir.data())) {
    return errno;
  }
  std::string path = tmp_dir + "/test.db";
  std::cout << "db_path " << path << std::endl;
  sqlite3* handle = nullptr;
  ret = sqlite3_open(path.c_str(), &handle);
  if (ret != SQLITE_OK) {
    std::cout << "Open failed with " << ret << std::endl;
    return ret;
  }
  ret = ExecuteQuery(handle, "PRAGMA journal_mode = WAL");
  if (ret != SQLITE_OK) {
    std::cout << "Enable WAL failed with " << ret << std::endl;
    return ret;
  }
  ret = ExecuteQuery(
      handle, "CREATE TABLE test_table (something TEXT, other TEXT)");
  if (ret != SQLITE_OK) {
    std::cout << "Create TABLE failed with " << ret << std::endl;
    return ret;
  }
  sqlite3_close(handle);
  std::atomic<bool> stop = false;
  std::thread t1(&KeepRunningQuery, &stop, path, "INSERT INTO test_table (something, other) VALUES (\"text1\", \"text2\")");
  std::thread t2(&KeepRunningQuery, &stop, path, "SELECT something, other FROM test_table");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  stop = true;
  t1.join();
  t2.join();

  return 0;
}