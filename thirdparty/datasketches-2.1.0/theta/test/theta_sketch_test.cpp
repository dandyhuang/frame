/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <catch.hpp>
#include <fstream>
#include <sstream>

#include <theta_sketch.hpp>

namespace datasketches {

#ifdef TEST_BINARY_INPUT_PATH
const std::string inputPath = TEST_BINARY_INPUT_PATH;
#else
const std::string inputPath = "test/";
#endif

TEST_CASE("theta sketch: empty", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  REQUIRE(update_sketch.is_empty());
  REQUIRE_FALSE(update_sketch.is_estimation_mode());
  REQUIRE(update_sketch.get_theta() == 1.0);
  REQUIRE(update_sketch.get_estimate() == 0.0);
  REQUIRE(update_sketch.get_lower_bound(1) == 0.0);
  REQUIRE(update_sketch.get_upper_bound(1) == 0.0);

  compact_theta_sketch compact_sketch = update_sketch.compact();
  REQUIRE(compact_sketch.is_empty());
  REQUIRE_FALSE(compact_sketch.is_estimation_mode());
  REQUIRE(compact_sketch.get_theta() == 1.0);
  REQUIRE(compact_sketch.get_estimate() == 0.0);
  REQUIRE(compact_sketch.get_lower_bound(1) == 0.0);
  REQUIRE(compact_sketch.get_upper_bound(1) == 0.0);
}

TEST_CASE("theta sketch: non empty no retained keys", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().set_p(0.001).build();
  update_sketch.update(1);
  //std::cerr << update_sketch.to_string();
  REQUIRE(update_sketch.get_num_retained() == 0);
  REQUIRE_FALSE(update_sketch.is_empty());
  REQUIRE(update_sketch.is_estimation_mode());
  REQUIRE(update_sketch.get_estimate() == 0.0);
  REQUIRE(update_sketch.get_lower_bound(1) == 0.0);
  REQUIRE(update_sketch.get_upper_bound(1) > 0);

  compact_theta_sketch compact_sketch = update_sketch.compact();
  REQUIRE(compact_sketch.get_num_retained() == 0);
  REQUIRE_FALSE(compact_sketch.is_empty());
  REQUIRE(compact_sketch.is_estimation_mode());
  REQUIRE(compact_sketch.get_estimate() == 0.0);
  REQUIRE(compact_sketch.get_lower_bound(1) == 0.0);
  REQUIRE(compact_sketch.get_upper_bound(1) > 0);
}

TEST_CASE("theta sketch: single item", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  update_sketch.update(1);
  REQUIRE_FALSE(update_sketch.is_empty());
  REQUIRE_FALSE(update_sketch.is_estimation_mode());
  REQUIRE(update_sketch.get_theta() == 1.0);
  REQUIRE(update_sketch.get_estimate() == 1.0);
  REQUIRE(update_sketch.get_lower_bound(1) == 1.0);
  REQUIRE(update_sketch.get_upper_bound(1) == 1.0);

  compact_theta_sketch compact_sketch = update_sketch.compact();
  REQUIRE_FALSE(compact_sketch.is_empty());
  REQUIRE_FALSE(compact_sketch.is_estimation_mode());
  REQUIRE(compact_sketch.get_theta() == 1.0);
  REQUIRE(compact_sketch.get_estimate() == 1.0);
  REQUIRE(compact_sketch.get_lower_bound(1) == 1.0);
  REQUIRE(compact_sketch.get_upper_bound(1) == 1.0);
}

TEST_CASE("theta sketch: resize exact", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  for (int i = 0; i < 2000; i++) update_sketch.update(i);
  REQUIRE_FALSE(update_sketch.is_empty());
  REQUIRE_FALSE(update_sketch.is_estimation_mode());
  REQUIRE(update_sketch.get_theta() == 1.0);
  REQUIRE(update_sketch.get_estimate() == 2000.0);
  REQUIRE(update_sketch.get_lower_bound(1) == 2000.0);
  REQUIRE(update_sketch.get_upper_bound(1) == 2000.0);

  compact_theta_sketch compact_sketch = update_sketch.compact();
  REQUIRE_FALSE(compact_sketch.is_empty());
  REQUIRE_FALSE(compact_sketch.is_estimation_mode());
  REQUIRE(compact_sketch.get_theta() == 1.0);
  REQUIRE(compact_sketch.get_estimate() == 2000.0);
  REQUIRE(compact_sketch.get_lower_bound(1) == 2000.0);
  REQUIRE(compact_sketch.get_upper_bound(1) == 2000.0);
}

TEST_CASE("theta sketch: estimation", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().set_resize_factor(update_theta_sketch::resize_factor::X1).build();
  const int n = 8000;
  for (int i = 0; i < n; i++) update_sketch.update(i);
  //std::cerr << update_sketch.to_string();
  REQUIRE_FALSE(update_sketch.is_empty());
  REQUIRE(update_sketch.is_estimation_mode());
  REQUIRE(update_sketch.get_theta() < 1.0);
  REQUIRE(update_sketch.get_estimate() == Approx((double) n).margin(n * 0.01));
  REQUIRE(update_sketch.get_lower_bound(1) < n);
  REQUIRE(update_sketch.get_upper_bound(1) > n);

  const uint32_t k = 1 << update_theta_sketch::builder::DEFAULT_LG_K;
  REQUIRE(update_sketch.get_num_retained() >= k);
  update_sketch.trim();
  REQUIRE(update_sketch.get_num_retained() == k);

  compact_theta_sketch compact_sketch = update_sketch.compact();
  REQUIRE_FALSE(compact_sketch.is_empty());
  REQUIRE(compact_sketch.is_ordered());
  REQUIRE(compact_sketch.is_estimation_mode());
  REQUIRE(compact_sketch.get_theta() < 1.0);
  REQUIRE(compact_sketch.get_estimate() == Approx((double) n).margin(n * 0.01));
  REQUIRE(compact_sketch.get_lower_bound(1) < n);
  REQUIRE(compact_sketch.get_upper_bound(1) > n);
}

TEST_CASE("theta sketch: deserialize update empty from java as base", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_update_empty_from_java.sk", std::ios::binary);
  auto sketchptr = theta_sketch::deserialize(is);
  REQUIRE(sketchptr->is_empty());
  REQUIRE_FALSE(sketchptr->is_estimation_mode());
  REQUIRE(sketchptr->get_num_retained() == 0);
  REQUIRE(sketchptr->get_theta() == 1.0);
  REQUIRE(sketchptr->get_estimate() == 0.0);
  REQUIRE(sketchptr->get_lower_bound(1) == 0.0);
  REQUIRE(sketchptr->get_upper_bound(1) == 0.0);
}

TEST_CASE("theta sketch: deserialize update empty from java as subclass", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_update_empty_from_java.sk", std::ios::binary);
  auto sketch = update_theta_sketch::deserialize(is);
  REQUIRE(sketch.is_empty());
  REQUIRE_FALSE(sketch.is_estimation_mode());
  REQUIRE(sketch.get_num_retained() == 0);
  REQUIRE(sketch.get_theta() == 1.0);
  REQUIRE(sketch.get_estimate() == 0.0);
  REQUIRE(sketch.get_lower_bound(1) == 0.0);
  REQUIRE(sketch.get_upper_bound(1) == 0.0);
}

TEST_CASE("theta sketch: deserialize update estimation from java as base", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_update_estimation_from_java.sk", std::ios::binary);
  auto sketchptr = theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketchptr->is_empty());
  REQUIRE(sketchptr->is_estimation_mode());
  REQUIRE(sketchptr->get_num_retained() == 5324);
  REQUIRE(sketchptr->get_estimate() == Approx(10000.0).margin(10000 * 0.01));
  REQUIRE(sketchptr->get_lower_bound(1) < 10000);
  REQUIRE(sketchptr->get_upper_bound(1) > 10000);
}

TEST_CASE("theta sketch: deserialize update estimation from java as subclass", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_update_estimation_from_java.sk", std::ios::binary);
  auto sketch = update_theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketch.is_empty());
  REQUIRE(sketch.is_estimation_mode());
  REQUIRE(sketch.get_num_retained() == 5324);
  REQUIRE(sketch.get_estimate() == Approx(10000.0).margin(10000 * 0.01));
  REQUIRE(sketch.get_lower_bound(1) < 10000);
  REQUIRE(sketch.get_upper_bound(1) > 10000);
}

TEST_CASE("theta sketch: deserialize compact empty from java as base", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_empty_from_java.sk", std::ios::binary);
  auto sketchptr = theta_sketch::deserialize(is);
  REQUIRE(sketchptr->is_empty());
  REQUIRE_FALSE(sketchptr->is_estimation_mode());
  REQUIRE(sketchptr->get_num_retained() == 0);
  REQUIRE(sketchptr->get_theta() == 1.0);
  REQUIRE(sketchptr->get_estimate() == 0.0);
  REQUIRE(sketchptr->get_lower_bound(1) == 0.0);
  REQUIRE(sketchptr->get_upper_bound(1) == 0.0);
}

TEST_CASE("theta sketch: deserialize compact empty from java as subclass", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_empty_from_java.sk", std::ios::binary);
  auto sketch = compact_theta_sketch::deserialize(is);
  REQUIRE(sketch.is_empty());
  REQUIRE_FALSE(sketch.is_estimation_mode());
  REQUIRE(sketch.get_num_retained() == 0);
  REQUIRE(sketch.get_theta() == 1.0);
  REQUIRE(sketch.get_estimate() == 0.0);
  REQUIRE(sketch.get_lower_bound(1) == 0.0);
  REQUIRE(sketch.get_upper_bound(1) == 0.0);
}

TEST_CASE("theta sketch: deserialize single item from java as base", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_single_item_from_java.sk", std::ios::binary);
  auto sketchptr = theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketchptr->is_empty());
  REQUIRE_FALSE(sketchptr->is_estimation_mode());
  REQUIRE(sketchptr->get_num_retained() == 1);
  REQUIRE(sketchptr->get_theta() == 1.0);
  REQUIRE(sketchptr->get_estimate() == 1.0);
  REQUIRE(sketchptr->get_lower_bound(1) == 1.0);
  REQUIRE(sketchptr->get_upper_bound(1) == 1.0);
}

TEST_CASE("theta sketch: deserialize single item from java as subclass", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_single_item_from_java.sk", std::ios::binary);
  auto sketch = compact_theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketch.is_empty());
  REQUIRE_FALSE(sketch.is_estimation_mode());
  REQUIRE(sketch.get_num_retained() == 1);
  REQUIRE(sketch.get_theta() == 1.0);
  REQUIRE(sketch.get_estimate() == 1.0);
  REQUIRE(sketch.get_lower_bound(1) == 1.0);
  REQUIRE(sketch.get_upper_bound(1) == 1.0);
}

TEST_CASE("theta sketch: deserialize compact estimation from java as base", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_estimation_from_java.sk", std::ios::binary);
  auto sketchptr = theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketchptr->is_empty());
  REQUIRE(sketchptr->is_estimation_mode());
  REQUIRE(sketchptr->is_ordered());
  REQUIRE(sketchptr->get_num_retained() == 4342);
  REQUIRE(sketchptr->get_theta() == Approx(0.531700444213199).margin(1e-10));
  REQUIRE(sketchptr->get_estimate() == Approx(8166.25234614053).margin(1e-10));
  REQUIRE(sketchptr->get_lower_bound(2) == Approx(7996.956955317471).margin(1e-10));
  REQUIRE(sketchptr->get_upper_bound(2) == Approx(8339.090301078124).margin(1e-10));

  // the same construction process in Java must have produced exactly the same sketch
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  const int n = 8192;
  for (int i = 0; i < n; i++) update_sketch.update(i);
  REQUIRE(sketchptr->get_num_retained() == update_sketch.get_num_retained());
  REQUIRE(sketchptr->get_theta() == Approx(update_sketch.get_theta()).margin(1e-10));
  REQUIRE(sketchptr->get_estimate() == Approx(update_sketch.get_estimate()).margin(1e-10));
  REQUIRE(sketchptr->get_lower_bound(1) == Approx(update_sketch.get_lower_bound(1)).margin(1e-10));
  REQUIRE(sketchptr->get_upper_bound(1) == Approx(update_sketch.get_upper_bound(1)).margin(1e-10));
  REQUIRE(sketchptr->get_lower_bound(2) == Approx(update_sketch.get_lower_bound(2)).margin(1e-10));
  REQUIRE(sketchptr->get_upper_bound(2) == Approx(update_sketch.get_upper_bound(2)).margin(1e-10));
  REQUIRE(sketchptr->get_lower_bound(3) == Approx(update_sketch.get_lower_bound(3)).margin(1e-10));
  REQUIRE(sketchptr->get_upper_bound(3) == Approx(update_sketch.get_upper_bound(3)).margin(1e-10));
  compact_theta_sketch compact_sketch = update_sketch.compact();
  // the sketches are ordered, so the iteration sequence must match exactly
  auto iter = sketchptr->begin();
  for (auto key: compact_sketch) {
    REQUIRE(*iter == key);
    ++iter;
  }
}

TEST_CASE("theta sketch: deserialize compact estimation from java as subclass", "[theta_sketch]") {
  std::ifstream is;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  is.open(inputPath + "theta_compact_estimation_from_java.sk", std::ios::binary);
  auto sketch = compact_theta_sketch::deserialize(is);
  REQUIRE_FALSE(sketch.is_empty());
  REQUIRE(sketch.is_estimation_mode());
  REQUIRE(sketch.get_num_retained() == 4342);
  REQUIRE(sketch.get_theta() == Approx(0.531700444213199).margin(1e-10));
  REQUIRE(sketch.get_estimate() == Approx(8166.25234614053).margin(1e-10));
  REQUIRE(sketch.get_lower_bound(2) == Approx(7996.956955317471).margin(1e-10));
  REQUIRE(sketch.get_upper_bound(2) == Approx(8339.090301078124).margin(1e-10));

  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  const int n = 8192;
  for (int i = 0; i < n; i++) update_sketch.update(i);
  REQUIRE(sketch.get_num_retained() == update_sketch.get_num_retained());
  REQUIRE(sketch.get_theta() == Approx(update_sketch.get_theta()).margin(1e-10));
  REQUIRE(sketch.get_estimate() == Approx(update_sketch.get_estimate()).margin(1e-10));
  REQUIRE(sketch.get_lower_bound(1) == Approx(update_sketch.get_lower_bound(1)).margin(1e-10));
  REQUIRE(sketch.get_upper_bound(1) == Approx(update_sketch.get_upper_bound(1)).margin(1e-10));
  REQUIRE(sketch.get_lower_bound(2) == Approx(update_sketch.get_lower_bound(2)).margin(1e-10));
  REQUIRE(sketch.get_upper_bound(2) == Approx(update_sketch.get_upper_bound(2)).margin(1e-10));
  REQUIRE(sketch.get_lower_bound(3) == Approx(update_sketch.get_lower_bound(3)).margin(1e-10));
  REQUIRE(sketch.get_upper_bound(3) == Approx(update_sketch.get_upper_bound(3)).margin(1e-10));
}

TEST_CASE("theta sketch: serialize deserialize stream and bytes equivalency", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  const int n = 8192;
  for (int i = 0; i < n; i++) update_sketch.update(i);

  // update sketch stream and bytes comparison
  {
    std::stringstream s(std::ios::in | std::ios::out | std::ios::binary);
    update_sketch.serialize(s);
    auto bytes = update_sketch.serialize();
    REQUIRE(bytes.size() == static_cast<size_t>(s.tellp()));
    for (size_t i = 0; i < bytes.size(); ++i) {
      REQUIRE(((char*)bytes.data())[i] == (char)s.get());
    }

    // deserialize as base class
    {
      s.seekg(0); // rewind
      auto deserialized_sketch_ptr1 = theta_sketch::deserialize(s);
      auto deserialized_sketch_ptr2 = theta_sketch::deserialize(bytes.data(), bytes.size());
      REQUIRE(bytes.size() == static_cast<size_t>(s.tellg()));
      REQUIRE(deserialized_sketch_ptr2->is_empty() == deserialized_sketch_ptr1->is_empty());
      REQUIRE(deserialized_sketch_ptr2->is_ordered() == deserialized_sketch_ptr1->is_ordered());
      REQUIRE(deserialized_sketch_ptr2->get_num_retained() == deserialized_sketch_ptr1->get_num_retained());
      REQUIRE(deserialized_sketch_ptr2->get_theta() == deserialized_sketch_ptr1->get_theta());
      REQUIRE(deserialized_sketch_ptr2->get_estimate() == deserialized_sketch_ptr1->get_estimate());
      REQUIRE(deserialized_sketch_ptr2->get_lower_bound(1) == deserialized_sketch_ptr1->get_lower_bound(1));
      REQUIRE(deserialized_sketch_ptr2->get_upper_bound(1) == deserialized_sketch_ptr1->get_upper_bound(1));
      // hash tables must be identical since they are restored from dumps, and iteration is deterministic
      auto iter = deserialized_sketch_ptr1->begin();
      for (auto key: *deserialized_sketch_ptr2) {
        REQUIRE(*iter == key);
        ++iter;
      }
    }

    // deserialize as subclass
    {
      s.seekg(0); // rewind
      update_theta_sketch deserialized_sketch1 = update_theta_sketch::deserialize(s);
      update_theta_sketch deserialized_sketch2 = update_theta_sketch::deserialize(bytes.data(), bytes.size());
      REQUIRE(bytes.size() == static_cast<size_t>(s.tellg()));
      REQUIRE(deserialized_sketch2.is_empty() == deserialized_sketch1.is_empty());
      REQUIRE(deserialized_sketch2.is_ordered() == deserialized_sketch1.is_ordered());
      REQUIRE(deserialized_sketch2.get_num_retained() == deserialized_sketch1.get_num_retained());
      REQUIRE(deserialized_sketch2.get_theta() == deserialized_sketch1.get_theta());
      REQUIRE(deserialized_sketch2.get_estimate() == deserialized_sketch1.get_estimate());
      REQUIRE(deserialized_sketch2.get_lower_bound(1) == deserialized_sketch1.get_lower_bound(1));
      REQUIRE(deserialized_sketch2.get_upper_bound(1) == deserialized_sketch1.get_upper_bound(1));
      // hash tables must be identical since they are restored from dumps, and iteration is deterministic
      auto iter = deserialized_sketch1.begin();
      for (auto key: deserialized_sketch2) {
        REQUIRE(*iter == key);
        ++iter;
      }
    }
  }

  // compact sketch stream and bytes comparison
  {
    std::stringstream s(std::ios::in | std::ios::out | std::ios::binary);
    update_sketch.compact().serialize(s);
    auto bytes = update_sketch.compact().serialize();
    REQUIRE(bytes.size() == static_cast<size_t>(s.tellp()));
    for (size_t i = 0; i < bytes.size(); ++i) {
      REQUIRE(((char*)bytes.data())[i] == (char)s.get());
    }

    // deserialize as base class
    {
      s.seekg(0); // rewind
      auto deserialized_sketch_ptr1 = theta_sketch::deserialize(s);
      auto deserialized_sketch_ptr2 = theta_sketch::deserialize(bytes.data(), bytes.size());
      REQUIRE(bytes.size() == static_cast<size_t>(s.tellg()));
      REQUIRE(deserialized_sketch_ptr2->is_empty() == deserialized_sketch_ptr1->is_empty());
      REQUIRE(deserialized_sketch_ptr2->is_ordered() == deserialized_sketch_ptr1->is_ordered());
      REQUIRE(deserialized_sketch_ptr2->get_num_retained() == deserialized_sketch_ptr1->get_num_retained());
      REQUIRE(deserialized_sketch_ptr2->get_theta() == deserialized_sketch_ptr1->get_theta());
      REQUIRE(deserialized_sketch_ptr2->get_estimate() == deserialized_sketch_ptr1->get_estimate());
      REQUIRE(deserialized_sketch_ptr2->get_lower_bound(1) == deserialized_sketch_ptr1->get_lower_bound(1));
      REQUIRE(deserialized_sketch_ptr2->get_upper_bound(1) == deserialized_sketch_ptr1->get_upper_bound(1));
      // the sketches are ordered, so the iteration sequence must match exactly
      auto iter = deserialized_sketch_ptr1->begin();
      for (auto key: *deserialized_sketch_ptr2) {
        REQUIRE(*iter == key);
        ++iter;
      }
    }

    // deserialize as subclass
    {
      s.seekg(0); // rewind
      compact_theta_sketch deserialized_sketch1 = compact_theta_sketch::deserialize(s);
      compact_theta_sketch deserialized_sketch2 = compact_theta_sketch::deserialize(bytes.data(), bytes.size());
      REQUIRE(bytes.size() == static_cast<size_t>(s.tellg()));
      REQUIRE(deserialized_sketch2.is_empty() == deserialized_sketch1.is_empty());
      REQUIRE(deserialized_sketch2.is_ordered() == deserialized_sketch1.is_ordered());
      REQUIRE(deserialized_sketch2.get_num_retained() == deserialized_sketch1.get_num_retained());
      REQUIRE(deserialized_sketch2.get_theta() == deserialized_sketch1.get_theta());
      REQUIRE(deserialized_sketch2.get_estimate() == deserialized_sketch1.get_estimate());
      REQUIRE(deserialized_sketch2.get_lower_bound(1) == deserialized_sketch1.get_lower_bound(1));
      REQUIRE(deserialized_sketch2.get_upper_bound(1) == deserialized_sketch1.get_upper_bound(1));
      // the sketches are ordered, so the iteration sequence must match exactly
      auto iter = deserialized_sketch1.begin();
      for (auto key: deserialized_sketch2) {
        REQUIRE(*iter == key);
        ++iter;
      }
    }
  }
}

TEST_CASE("theta sketch: deserialize update single item buffer overrun", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  update_sketch.update(1);
  theta_sketch::vector_bytes bytes = update_sketch.serialize();
  REQUIRE_THROWS_AS(update_theta_sketch::deserialize(bytes.data(), 7), std::out_of_range);
  REQUIRE_THROWS_AS(update_theta_sketch::deserialize(bytes.data(), bytes.size() - 1), std::out_of_range);
}

TEST_CASE("theta sketch: deserialize compact single item buffer overrun", "[theta_sketch]") {
  update_theta_sketch update_sketch = update_theta_sketch::builder().build();
  update_sketch.update(1);
  theta_sketch::vector_bytes bytes = update_sketch.compact().serialize();
  REQUIRE_THROWS_AS(compact_theta_sketch::deserialize(bytes.data(), 7), std::out_of_range);
  REQUIRE_THROWS_AS(compact_theta_sketch::deserialize(bytes.data(), bytes.size() - 1), std::out_of_range);
}

} /* namespace datasketches */
