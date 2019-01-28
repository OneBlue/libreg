#pragma once

#include "Tester.h"
namespace libreg
{
  namespace test
  {
    class TestRegistryOperations : public Tester
    {
    public:
      TestRegistryOperations();
      virtual ~TestRegistryOperations() = default;

      virtual void RunImpl() override;

    private:
    };
  }
}