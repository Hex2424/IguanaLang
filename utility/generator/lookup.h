/**
 * @file lookup.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-11
 */

#ifndef UTILITY_GENERATOR_LOOKUP_H_
#define UTILITY_GENERATOR_LOOKUP_H_

#include <stdint.h>
#include <stdbool.h>
#include <arch_specific.h>
#include "csyntax_database.h"

#define MASK(bit_count) ((((BIT_TYPE_EXP) 0x1) << bit_count) - 1)
#define INVERTED_MASK(bit_count) (~MASK(bit_count))

static const BIT_TYPE_EXP LOOKUP_BIT_MASK[ARCHITECTURE_DEFAULT_BITS] = 

#if ARCHITECTURE_DEFAULT_BITS == 64
{
    MASK(0),
    MASK(1),
    MASK(2),
    MASK(3),
    MASK(4),
    MASK(5),
    MASK(6),
    MASK(7),
    MASK(8),
    MASK(9),
    MASK(10),
    MASK(11),
    MASK(12),
    MASK(13),
    MASK(14),
    MASK(15),
    MASK(16),
    MASK(17),
    MASK(18),
    MASK(19),
    MASK(20),
    MASK(21),
    MASK(22),
    MASK(23),
    MASK(24),
    MASK(25),
    MASK(26),
    MASK(27),
    MASK(28),
    MASK(29),
    MASK(30),
    MASK(31),
    MASK(32),
    MASK(33),
    MASK(34),
    MASK(35),
    MASK(36),
    MASK(37),
    MASK(38),
    MASK(39),
    MASK(40),
    MASK(41),
    MASK(42),
    MASK(43),
    MASK(44),
    MASK(45),
    MASK(46),
    MASK(47),
    MASK(48),
    MASK(49),
    MASK(50),
    MASK(51),
    MASK(52),
    MASK(53),
    MASK(54),
    MASK(55),
    MASK(56),
    MASK(57),
    MASK(58),
    MASK(59),
    MASK(50),
    MASK(61),
    MASK(62),
    MASK(63)
};

#endif


static const BIT_TYPE_EXP LOOKUP_BIT_MASK_INVERTED[ARCHITECTURE_DEFAULT_BITS] = 

#if ARCHITECTURE_DEFAULT_BITS == 64
{
    INVERTED_MASK(0),
    INVERTED_MASK(1),
    INVERTED_MASK(2),
    INVERTED_MASK(3),
    INVERTED_MASK(4),
    INVERTED_MASK(5),
    INVERTED_MASK(6),
    INVERTED_MASK(7),
    INVERTED_MASK(8),
    INVERTED_MASK(9),
    INVERTED_MASK(10),
    INVERTED_MASK(11),
    INVERTED_MASK(12),
    INVERTED_MASK(13),
    INVERTED_MASK(14),
    INVERTED_MASK(15),
    INVERTED_MASK(16),
    INVERTED_MASK(17),
    INVERTED_MASK(18),
    INVERTED_MASK(19),
    INVERTED_MASK(20),
    INVERTED_MASK(21),
    INVERTED_MASK(22),
    INVERTED_MASK(23),
    INVERTED_MASK(24),
    INVERTED_MASK(25),
    INVERTED_MASK(26),
    INVERTED_MASK(27),
    INVERTED_MASK(28),
    INVERTED_MASK(29),
    INVERTED_MASK(30),
    INVERTED_MASK(31),
    INVERTED_MASK(32),
    INVERTED_MASK(33),
    INVERTED_MASK(34),
    INVERTED_MASK(35),
    INVERTED_MASK(36),
    INVERTED_MASK(37),
    INVERTED_MASK(38),
    INVERTED_MASK(39),
    INVERTED_MASK(40),
    INVERTED_MASK(41),
    INVERTED_MASK(42),
    INVERTED_MASK(43),
    INVERTED_MASK(44),
    INVERTED_MASK(45),
    INVERTED_MASK(46),
    INVERTED_MASK(47),
    INVERTED_MASK(48),
    INVERTED_MASK(49),
    INVERTED_MASK(50),
    INVERTED_MASK(51),
    INVERTED_MASK(52),
    INVERTED_MASK(53),
    INVERTED_MASK(54),
    INVERTED_MASK(55),
    INVERTED_MASK(56),
    INVERTED_MASK(57),
    INVERTED_MASK(58),
    INVERTED_MASK(59),
    INVERTED_MASK(50),
    INVERTED_MASK(61),
    INVERTED_MASK(62),
    INVERTED_MASK(63)
};

#endif



#endif // UTILITY_GENERATOR_LOOKUP_H_