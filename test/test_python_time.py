"""
/// @file   test_python_time.py
///
/// @brief  tests the time classes as exported to python
///
/// @authors    Maarten P. Scholl
/// @date       2019-09-04
/// @copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
///             Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing,
///             software distributed under the License is distributed on an "AS
///             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
///             express or implied. See the License for the specific language
///             governing permissions and limitations under the License.
///
///             You may obtain instructions to fulfill the attribution
///             requirements in CITATION.cff
///
"""
import esl

__ = """
def test_time_built():
    if not hasattr(esl.time, "time_interval"):
        pytest.fail("module missing: esl.time.time_interval")
    if not hasattr(esl.time, "time_duration"):
        pytest.fail("module missing: esl.time.time_duration")
    if not hasattr(esl.time, "time_point"):
        pytest.fail("module missing: esl.time.time_point")


def test_time_point():
    point = esl.time.time_point(0)
    assert point is not None


def test_time_duration():
    duration = esl.time.time_duration(0)
    assert duration is not None


def test_time_step():
    step = esl.time.time_interval(0, 1)
    assert step is not None
    assert not step.empty()
    assert step.singleton()
    assert step.degenerate()
    assert step.contains(0)

    r = "[0,1)"
    assert str(step) == r
    assert repr(step) == r
"""