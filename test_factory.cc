#include "factory/query_manager/query_base.h"
#include "factory/query_manager/query_factory.h"

int main() {
  auto query = GET_QUERY("Query805");
  query->QueryData();
  return 0;
}