#pragma once

#include "../config.hpp"
#include "../Bits.hpp"
#include "../Wire.hpp"
#include "Unit.hpp"
#include "Plexers.hpp"

/// input signal: input, WriteEnabled(WE), clock(tick)
/// output signal: output
/// when tick, update the output by the input.
struct Register: SequentialLogicUnit {
  InputWire<32> input;
  InputWire<1> WriteEnabled;
  OutputWire<32> output;

  Register() = default;
  ~Register() = default;

  auto setInput(const InputWire<32> &in) -> void {
    input = in;
  }

  auto setWE(const InputWire<1> &WE) -> void {
    WriteEnabled = WE;
  }

  auto tick() -> void {
    if (WriteEnabled.get()[0])
      output.set(input.get());
  }
};

/// input signal:
/// - read: rs1, rs2
/// - write: rd, data, WriteEnabled
/// output signal:
/// - rdata1(reg[rs1]), rdata2(reg[rs2])
/// using index(rs1/rs2/rd) as the @sel signal of Mux/DMux
/// to read/write the specified register.
struct RegisterFile: CombinationalLogicUnit {
  Register x[32];
  Mux<32, 5> ReadMux1, ReadMux2;
  DMux<1, 5> WriteDMux;

  InputWire<32> wdata;
  OutputWire<32> rdata1, rdata2;

  RegisterFile() {
    [this]<u32... Index>(std::integer_sequence<u32, Index...>) {
      ReadMux1.setInput(x[Index].output...);
      ReadMux2.setInput(x[Index].output...);
    }(std::make_integer_sequence<u32, 32>{});
    rdata1 = ReadMux1.output;
    rdata2 = ReadMux2.output;

    [this]<u32... Index>(std::integer_sequence<u32, Index...>) {
      ((x[Index + 1].setWE(WriteDMux.output[Index + 1])), ...);
    }(std::make_integer_sequence<u32, 31>{});
  }
  ~RegisterFile() = default;

  auto setRSel(const InputWire<5> &rs1,
               const InputWire<5> &rs2) -> void {
    ReadMux1.setSel(rs1);
    ReadMux2.setSel(rs2);
  }
  auto setWE(const InputWire<1> &WE) -> void {
    WriteDMux.setInput(WE);
  }
  auto setWSel(const InputWire<5> &rd) -> void {
    WriteDMux.setSel(rd);
  }
  auto setWData(const InputWire<32> &wb) -> void {
    [this, &wb]<u32... Index>(std::integer_sequence<u32, Index...>) {
      ((x[Index + 1].setInput(wb)), ...);
    }(std::make_integer_sequence<u32, 31>{});
  }

  auto exec() -> void {
    ReadMux1.exec();
    ReadMux2.exec();
    WriteDMux.exec();
  }
};
