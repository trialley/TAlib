#pragma once

namespace TA {

class noncopyable {
 protected:
  noncopyable() = default;
  ~noncopyable() = default;

 private:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
};

}  // namespace TA
