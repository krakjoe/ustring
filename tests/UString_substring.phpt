--TEST--
Test UString::substring
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "ABCDEFG";

$string = new UString($std);

var_dump((string) $string->substring(1),
         (string) $string->substring(2, 1));
?>
--EXPECT--
string(6) "BCDEFG"
string(1) "C"

