#pragma once

#include "config.hpp"
#include "Bits.hpp"

namespace {
  template <u32 Width = 32u>
  struct InputWireImpl {
    virtual ~InputWireImpl() = default;
    virtual auto get() const -> Bits<Width> = 0;
  };

  template <u32 Width = 32u>
  struct OutputWireImpl {
    virtual ~OutputWireImpl() = default;
    virtual auto set(const Bits<Width> &value) -> void = 0;
  };

  template <u32 Width = 32u>
  struct WireImpl: InputWireImpl<Width>, OutputWireImpl<Width> {
    WireImpl() = default;
    ~WireImpl() = default;
    auto get() const -> Bits<Width> { return _value; }
    auto set(const Bits<Width> &value) -> void { _value = value; }
  private:
    Bits<Width> _value;
  };
}

template <u32 Width>
struct InputWire {
  std::shared_ptr<InputWireImpl<Width>> impl;

  InputWire(): impl(std::make_shared<WireImpl<Width>>()) {}
  InputWire(const OutputWire<Width> &);
  auto operator= (const OutputWire<Width> &) -> InputWire<Width>&;

  auto get() const -> Bits<Width> { return impl->get(); }
  auto getWire() const -> std::shared_ptr<WireImpl<Width>> {
    return std::dynamic_pointer_cast<WireImpl<Width>>(impl);
  }
};

template <u32 Width>
struct OutputWire {
  std::shared_ptr<OutputWireImpl<Width>> impl;

  OutputWire(): impl(std::make_shared<WireImpl<Width>>()) {}
  OutputWire(const InputWire<Width> &);
  auto operator= (const InputWire<Width> &) -> OutputWire<Width>&;

  auto set(const Bits<Width> &value) -> void { impl->set(value); }
  auto getWire() const -> std::shared_ptr<WireImpl<Width>> {
    return std::dynamic_pointer_cast<WireImpl<Width>>(impl);
  }
};

template <u32 Width>
InputWire<Width>::InputWire(const OutputWire<Width> &ow):
  impl(ow.getWire()) {}

template <u32 Width>
auto InputWire<Width>::operator= (const OutputWire<Width> &ow)
  -> InputWire<Width>& {
  impl = ow.getWire();
  return *this;
}

template <u32 Width>
OutputWire<Width>::OutputWire(const InputWire<Width> &iw):
  impl(iw.getWire()) {}

template <u32 Width>
auto OutputWire<Width>::operator= (const InputWire<Width> &iw)
  -> OutputWire<Width>& {
  impl = iw.getWire();
  return *this;
}
