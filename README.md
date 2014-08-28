[![Build Status](https://travis-ci.org/krakjoe/ustring.svg?branch=master)](https://travis-ci.org/krakjoe/ustring)

ustring
=======

Exposing an interface to [`UnicodeString`](http://icu-project.org/apiref/icu4c/classicu_1_1UnicodeString.html) for upcoming PHP7.

The following interface will be available:

```php
<?php
abstract class UString implements Iterable {
    public function __construct([string $string [, string $codepage] ]);
    
    /** 
    * @return int
    **/
    public function length();
    
    /** 
    * @return UString
    **/
    public function toUpper(void);
    
    /** 
    * @return UString
    **/
    public function toLower(void);
    
    /** 
    * @return UString
    **/
    public function reverse(void);
    
    /**
    * @return UString
    **/
    public function trim(void);
    
    /**
    * @return UString
    **/
    public function replace(UString $search, UString $replace);
    
    /**
    * @return UString
    **/
    public function replaceSlice(UString $text [, int $start [, int $length]])
    
    /**
    * @return bool
    **/
    public function contains(UString $text);
    
    /**
    * @return bool
    **/
    public function startsWith(UString $needle);
    
    /**
    * @return bool
    **/
    public function endsWith(UString $needle);
    
    /**
    * @return mixed
    **/
    public function indexOf(UString $needle [, int $offset]);
    
    /**
    * @return mixed
    **/
    public function lastIndexOf(UString $needle [, int $offset]);
    
    /**
    * @return UString[]
    **/
    public function chunk(int $length);
    
    /**
    * @return UString
    **/
    public function repeat(int $count);
    
    /**
    * @return UString
    **/
    public function pad(int $length [, UString $pad = ' ' [, int $mode = STR_PAD_RIGHT ] ]);
    
    /**
    * @return UString[]
    **/
    public function split(UString $delimiter [, int $limit = NULL]);
    
    /**
    * @return UString
    **/
    public function charAt(int $index);
    
    /**
    * @return UString
    **/
    public function substring(int $start [, int $length]);
    
    /**
    * @return string
    **/
    public function getCodepage(void);
    
    /**
    * @return string
    **/
    public static function getDefaultCodepage();
    
    /**
    * @return void
    **/
    public static function setDefaultCodepage(string $codepage);
}

/**
* A convenience shortcut for UString constructor is registered by the extension
* @return UString
**/
function u(string $value);
?>
```

*Note: UString is interchangable with zend strings for method parameters and can be cast for output/conversion to zend strings*

**This extension requires PHP7 and ICU (with headers)**
