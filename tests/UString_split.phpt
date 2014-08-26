--TEST--
Test UString::split
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = new UString("、リンゴ、オレンジ、バナナ、、");

function print_array(array $arr)
{
    $first = true;
	echo "[";
    foreach ($arr as $elem) {
        if ($first) {
            $first = false;
        } else {
            echo ", ";
        }
        echo "\"$elem\"";
    }
    echo "]" . PHP_EOL;
}

// No such delimeter
print_array($std->split(","));
echo PHP_EOL;

// Check both strings and UStrings work
print_array($std->split("、"));
print_array($std->split(new UString("、")));
echo PHP_EOL;

// Test limits
print_array($std->split("、", 1));
print_array($std->split("、", 2));
print_array($std->split("、", 3));
print_array($std->split("、", 4));
echo PHP_EOL;

// Test multi-character delimiters
print_array($std->split("ナナ"));
echo PHP_EOL;

// Test zero-character delimiters
// TODO - add this into separate error test case
//print_array($std->split(""));

?>
--EXPECTF--
["、リンゴ、オレンジ、バナナ、、"]

["", "リンゴ", "オレンジ", "バナナ", "", ""]
["", "リンゴ", "オレンジ", "バナナ", "", ""]

["、リンゴ、オレンジ、バナナ、、"]
["", "リンゴ、オレンジ、バナナ、、"]
["", "リンゴ", "オレンジ、バナナ、、"]
["", "リンゴ", "オレンジ", "バナナ、、"]

["、リンゴ、オレンジ、バ", "、、"]

