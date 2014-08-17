--TEST--
Test UString::repeat
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "hi";
$string = new UString($std);
var_dump((string)$string->repeat(3));
?>
--EXPECT--
string(6) "hihihi"


