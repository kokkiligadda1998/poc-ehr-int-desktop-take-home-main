#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>
#include <optional>

template<class T>
T BaseName(T const & path, T const & delims = "/\\")
{
  return path.substr(path.find_last_of(delims) + 1);
}
template<class T>
T RemoveExtension(T const & filename)
{
  typename T::size_type const p(filename.find_last_of('.'));
  return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}


struct ScannedApplication {
  std::string window_title;
  std::string procedure_path;
  std::string procedure_name;
};

struct ApplicationStatusCriteria {
  std::string applicationName;
  std::string windowTitle;
};

struct ApplicationStatusDimensions {
  int x, y, width, height;
};

struct ApplicationStatus {
  std::string status; // "open", "hidden", "closed"
  std::optional<ApplicationStatusDimensions> dimensions;
};

#endif // HELPERS_HPP
