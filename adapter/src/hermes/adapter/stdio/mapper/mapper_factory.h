#ifndef HERMES_ADAPTER_FACTORY_H
#define HERMES_ADAPTER_FACTORY_H

#include <hermes/adapter/singleton.h>
#include <hermes/adapter/stdio/common/enumerations.h>

#include "abstract_mapper.h"
#include "balanced_mapper.h"
namespace hermes::adapter::stdio {
class MapperFactory {
 public:
  std::shared_ptr<AbstractMapper> Get(const MapperType &type) {
    switch (type) {
      case MapperType::BALANCED: {
        return hermes::adapter::Singleton<BalancedMapper>::GetInstance();
      }
      default: {
        // TODO(hari) throw: Mapper not implemented
      }
    }
  }
};
}  // namespace hermes::adapter::stdio
#endif  // HERMES_ADAPTER_FACTORY_H