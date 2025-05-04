#pragma once

enum class ScriptCellType {
  Void,
  Number,
  String,
  FnHandle,
};

using ScriptCell = double;