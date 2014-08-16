--TEST--
Test UString write dimension
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);
$replace = "krakjoe";

for ($i = 0; $i < $string->length(); $i++) {
    $string[$i] = $replace[$i];
}

var_dump((string)$string);
?>
--EXPECT--
string(7) "krakjoe"
