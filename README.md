[![Build Status](https://travis-ci.org/krakjoe/ustring.svg?branch=master)](https://travis-ci.org/krakjoe/ustring)

ustring
=======

Provides a simple interface to work with Unicode strings in upcoming PHP7.


```php
<?php
/** 
* @property int $length The number of unicode code points (read-only)
**/
class UString implements Traversable {
    /**
     * Shall create a UString with the given value and codepage.
     * If no codepage is provided, the default codepage will be used.
     * The default codepage is set to UTF-8 by default
     * @see UString::setDefaultCodepage
     * @see UString::getDefaultCodepage
     */
    public function __construct([string $string [, string $codepage] ]);
    
    /** 
     * Shall return a new UString whose characters are converted to upper case
     */
    public function toUpper(): UString;
    
    /** 
     * Shall return a new UString whose characters are converted to lower case
     */
    public function toLower(): UString;
    
    /** 
     * Shall return a new UString whose characters have been reversed
     */
    public function reverse(): UString;
    
    /**
     * Shall return a new UString with the leading and trailing space removed
     */
    public function trim(): UString;
    
    /**
     * Shall return a new UString with every occurence of $search having been replaced with $replace
     */
    public function replace(UString $search, UString $replace): UString;
    
    /**
     * Shall return a new UString with the substring starting at $start and having length $length replaced by $text.
     */
    public function replaceSlice(UString $text, int $start, int $length): UString;
    
    /**
     * Shall return positively if this contains $text
     */
    public function contains(UString $text): bool;
    
    /**
     * Shall return positively if this starts with $needle
     */
    public function startsWith(UString $needle): bool;
    
    /**
     * Shall return positively if this ends with $needle
     */
    public function endsWith(UString $needle): bool;
    
    /**
     * Shall return the index of the first occurence of $needle, with the search starting at $offset
     * @return mixed
     */
    public function indexOf(UString $needle, int $offset = 0);
    
    /**
     * Shall return the index of the last occurence of $needle, with the search starting at $offset
     * @return mixed
     */
    public function lastIndexOf(UString $needle, int $offset = 0);
    
    /**
     * Shall return an array of UString containing chunks of this, each with a maximum length of $length
     * @return UString[]
     */
    public function chunk(int $length): array;
    
    /**
     * Shall return a new UString with the contents of this repeated $count times
     */
    public function repeat(int $count): UString;
    
    /**
	 * Shall return a new UString padded to a given length with the pad character
     */
    public function pad(int $length, UString $pad = ' ', int $mode = STR_PAD_RIGHT): UString;
    
    /**
	 * Shall split a UString by the delimeter up to the limit, if specified, returning an array of UStrings
     * @return UString[]
     */
    public function split(UString $delimiter, int $limit = NULL): array;
    
    /**
	 * Shall return a new UString containing the single code point at the given index
     */
    public function charAt(int $index): UString;
    
    /**
     * Shall return a new UString containing a substring of this, starting at $start with a maximum length of $length
     * If $length is not provided shall use UnicodeString::length() - $start to determine the length of the substring to return
     */
    public function substring(int $start, int $length): UString;
    
    /**
     * Shall return the codepage of this
     */
    public function getCodepage(): string;
    
    /**
     * Shall return the default codepage
     */
    public static function getDefaultCodepage(): string;
    
    /**
     * Shall set the default codepage
     * @return void
     */
    public static function setDefaultCodepage(string $codepage);
}

/**
 * A convenience shortcut for UString constructor is registered by the extension
 */
function u(string $value [, string $codepage]): UString;
?>
```

This API is derived, in part, from work done by Nikita Poppov, which can be found here: https://github.com/nikic/scalar_objects/blob/master/doc/string_api.md

*Note: UString is interchangable with zend strings for method parameters and can be cast for output/conversion to zend strings*

**This extension requires PHP7 and ICU (with headers)**
