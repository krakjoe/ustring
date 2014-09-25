--TEST--
Test UString::$length
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->length);
var_dump(strlen($string));
echo PHP_EOL;

var_dump(isset($string->length));
var_dump(empty($string->length));
var_dump(property_exists($string, 'length'));

echo PHP_EOL;
$empty = u("");

var_dump(isset($empty->length));
var_dump(empty($empty->length));
var_dump(property_exists($empty, 'length'));

echo PHP_EOL;
var_dump(new ReflectionProperty("UString", "length"));

$string->length = 3;
?>
--EXPECTF--
int(7)
int(21)

bool(true)
bool(false)
bool(true)

bool(true)
bool(true)
bool(true)

object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(6) "length"
  ["class"]=>
  string(7) "UString"
}

Fatal error: Writing to length property is unsupported in %s on line %d
