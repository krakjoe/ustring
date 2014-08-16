--TEST--
Test UString concat operator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

$result = $string .= ", whatever";

var_dump((string) $result);
?>
--EXPECTF--
string(17) "%s, whatever"

