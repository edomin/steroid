function(st_capitalize ST_STR ST_STR_CAPITALIZED)
    string(SUBSTRING ${ST_STR} 0 1 ST_FIRST_LETTER)
    string(TOUPPER ${ST_FIRST_LETTER} ST_FIRST_LETTER_CAPITALIZED)
    string(SUBSTRING ${ST_STR} 1 -1 ST_REMAINING_LETTERS)

    set(${ST_STR_CAPITALIZED}
        "${ST_FIRST_LETTER_CAPITALIZED}${ST_REMAINING_LETTERS}"
        PARENT_SCOPE
    )
endfunction()
