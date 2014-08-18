--TEST--
Test UString::reverse
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "ABCDEFG";

$string = new UString($std);

var_dump((string) $string->reverse());
?>
--EXPECT--
string(7) "GFEDCBA"
