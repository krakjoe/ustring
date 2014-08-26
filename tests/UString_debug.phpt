--TEST--
Test UString debug info
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(new UString("こんにちは世界"));
?>
--EXPECT--
object(UString)#1 (7) {
  [0]=>
  string(3) "こ"
  [1]=>
  string(3) "ん"
  [2]=>
  string(3) "に"
  [3]=>
  string(3) "ち"
  [4]=>
  string(3) "は"
  [5]=>
  string(3) "世"
  [6]=>
  string(3) "界"
}



