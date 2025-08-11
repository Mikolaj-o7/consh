#include <iostream>
#include <string>
#include <optional>
#include <format>
#include <climits>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include "lexer.hpp"

[[nodiscard]] std::optional<std::string> getenv_str(const char *name) {
  if (const char *val = std::getenv(name); val && *val) {
    return std::string(val);
  }
  return std::nullopt;
}

[[nodiscard]] std::optional<std::string> get_username() {
  if (std::optional<std::string> val = getenv_str("USER")) return val;
  if (std::optional<std::string> val = getenv_str("LOGNAME")) return val;

  if (const passwd *pw = getpwuid(getuid());
    pw && pw->pw_name && *pw->pw_name) {
    return std::string(pw->pw_name);
  }
  return std::nullopt;
}

[[nodiscard]] std::optional<std::string> get_home_directory() {
  if (std::optional<std::string> val = getenv_str("HOME")) return val;

  if (const passwd *pw = getpwuid(getuid());
    pw && pw->pw_dir && *pw->pw_dir) {
    return std::string(pw->pw_dir);
  }
  return std::nullopt;
}

[[nodiscard]] std::optional<std::string> get_current_working_directory() {
  char buf[PATH_MAX];
  const ssize_t len = readlink("/proc/self/cwd", buf, sizeof(buf) - 1);
  if (len == -1) return std::nullopt;

  buf[len] = '\0';

  constexpr char deleted_suffix[] = " (deleted)";
  constexpr size_t deleted_suffix_len = sizeof(deleted_suffix) - 1;

  bool deleted = false;
  size_t path_len = static_cast<size_t>(len);

  if (path_len > deleted_suffix_len &&
      std::char_traits<char>::compare(buf + path_len - deleted_suffix_len, deleted_suffix, deleted_suffix_len) == 0) {
    deleted = true;
    path_len -= deleted_suffix_len;
    buf[path_len] = '\0';
  } else {
    struct stat st{};
    if (stat(buf, &st) != 0) {
      deleted = true;
    }
  }

  std::string cwd_str(buf, path_len);

  if (const std::optional<std::string> home = get_home_directory()) {
    const std::string &home_str = *home;
    if (cwd_str.size() >= home_str.size() &&
        cwd_str.compare(0, home_str.size(), home_str) == 0 &&
        (cwd_str.size() == home_str.size() || cwd_str[home_str.size()] == '/')) {
      cwd_str.replace(0, home_str.size(), "~");
    }
  }

  if (deleted) {
    cwd_str += " (deleted)";
  }

  return cwd_str;
}

std::string get_prompt() {
  std::string username = get_username().value_or("unknown");
  std::string cwd = get_current_working_directory().value_or("(deleted)");
  return std::format("{}@consh:{}$ ", username, cwd);
}

int main() {
  while (true) {
    std::cout << get_prompt();
    std::cout.flush();

    std::string line;
    if (!std::getline(std::cin, line)) break;

    for (auto& t : tokenize(line)) {
      std::cout << (int)t.type << ": " << t.value << std::endl;
    }
  }
  return 0;
}
