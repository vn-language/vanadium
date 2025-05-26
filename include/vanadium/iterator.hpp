#ifndef INCLUDE_VANADIUM_ITERATOR_HPP_
#define INCLUDE_VANADIUM_ITERATOR_HPP_

#include <vector>
namespace vanadium {

template <typename VT> class VecIterator {
public:
  VecIterator(std::vector<VT> vector) : elements(vector) {};
  VecIterator(VecIterator &&) = default;
  VecIterator(const VecIterator &) = default;
  VecIterator &operator=(VecIterator &&) = default;
  VecIterator &operator=(const VecIterator &) = default;
  ~VecIterator() = default;

  const VT next() {
    auto last = elements.back();
    elements.pop_back();
    return last;
  };

  const bool empty() { return elements.empty(); };

  const std::vector<VT> iter() { return elements; }

private:
  std::vector<VT> elements;
};

} // namespace vanadium

#endif // INCLUDE_VANADIUM_ITERATOR_HPP_
