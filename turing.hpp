#ifndef CPU_HPP
#define CPU_HPP

#pragma once

#include <cstdint>
#include <ostream>
#include <vector>
#include <array>
#include <string>
#include <bitset>

#define ASSERT(EXPR, MSG) if (!(EXPR)) throw std::runtime_error(MSG);

#define NON_CMA_ABLE(T)             \
public:                             \
  T(const T &) = delete;            \
  T(T &&) noexcept = delete;        \
                                    \
public:                             \
  T operator=(const T &) = delete;

namespace Turing {
  class Ram final {
  NON_CMA_ABLE(Ram)

  public:
    Ram() = default;

  public:
    uint8_t Read(uint8_t addr) {
      if (addr >= 0x0 && addr <= 0xFF)
        return mRegisters[addr];
      return 0;
    }

    void Write(uint8_t addr, uint8_t data) {
      if (addr >= 0x0 && addr <= 0xFF)
        mRegisters[addr] = data;
    }

    void Dump(uint8_t from, uint8_t to) {
      for (uint8_t addr = from; addr < to; addr++)
        std::cout << std::bitset<8>(mRegisters[addr]) << ' ' << static_cast<int>(addr) << '\n';
      std::cout << '\n';
    }

  private:
    std::array<uint8_t, 0xFF> mRegisters{};
  };

  enum OpCode : uint8_t {
    NOP = 0x0,
    LDX = 0x1,
    LDY = 0x2,
    STX = 0x3,
    STY = 0x4,
    ADD = 0x5,
    SUB = 0x6,
    JMP = 0x7,
  };

  class Cpu final {
  NON_CMA_ABLE(Cpu)

  public:
    explicit Cpu(Ram &ram) : mRam(ram) {}

  public:
    void Run(const std::vector<uint8_t> &program) {
      ASSERT(0x10 > program.size(), "Program max size 0x10")

      auto size = static_cast<uint8_t>(program.size());

      for (uint8_t addr = 0x0; addr < size; addr++)
        mRam.Write(addr, program[addr]);

      mRam.Dump(0x0, size);
      Dump();
    }

    void Clock() {
      mOpCode = mRam.Read(mProgramCounter);
      mProgramCounter++;

      if (mInstructions[mOpCode].Operation)
        (this->*mInstructions[mOpCode].Operation)();
    }

    void Fetch() {
      mFetched = mRam.Read(mStackPointer);
    }

    void Dump() {
      std::cout << "Register X: " << static_cast<int>(mRegisterX) << '\n'
                << "Register Y: " << static_cast<int>(mRegisterY) << '\n'
                << "Overflow: " << static_cast<int>(mOverflow) << '\n'
                << "Underflow: " << static_cast<int>(mUnderflow) << '\n'
                << "Fetched: " << static_cast<int>(mFetched) << '\n'
                << "Stack Pointer: " << static_cast<int>(mStackPointer) << '\n'
                << "Program Counter: " << static_cast<int>(mProgramCounter) << '\n'
                << "Op Code: " << static_cast<int>(mOpCode) << '\n'
                << "Tmp: " << static_cast<int>(mTmp) << '\n'
                << '\n';
    }

  private:
    void LDX() {
      Fetch();

      mRegisterX = mFetched;
    }
    void LDY() {
      Fetch();

      mRegisterY = mFetched;
    }
    void STX() {
      mRam.Write(mStackPointer, mRegisterX);
    }
    void STY() {
      mRam.Write(mStackPointer, mRegisterY);
    }
    void ADD() {
      mTmp = mRegisterX + mRegisterY;

      if (mTmp > 0xFF) mOverflow = true;
    }
    void SUB() {
      mTmp = mRegisterX - mRegisterY;

      if (mTmp < 0x0) mUnderflow = true;
    }
    void JMP() {
      mProgramCounter = mStackPointer;
    }

  private:
    struct Instruction {
      std::string Name;
      void (Cpu::*Operation)() = nullptr;
    };

  private:
    Ram &mRam;

    uint8_t mRegisterX{};
    uint8_t mRegisterY{};

    bool mOverflow{};
    bool mUnderflow{};

    uint8_t mFetched{};
    uint8_t mStackPointer{};
    uint8_t mProgramCounter{};
    uint8_t mOpCode{};

    uint8_t mTmp{};

    std::vector<Instruction> mInstructions{
        {"NOP", nullptr},
        {"LDX", &Cpu::LDX},
        {"LDY", &Cpu::LDY},
        {"STX", &Cpu::STX},
        {"STY", &Cpu::STY},
        {"ADD", &Cpu::ADD},
        {"SUB", &Cpu::SUB},
        {"JMP", &Cpu::JMP},
    };
  };
}

#endif