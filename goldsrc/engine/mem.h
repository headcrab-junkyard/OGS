/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief dynamic memory management functions

#pragma once

/// Allocate memory block
void *Mem_Alloc(size_t size);

/// Allocate number * size bytes and zero it
void *Mem_Calloc(int number, size_t size);

/// Reallocate memory block
void *Mem_Realloc(void *ptr, size_t size);

/// Deallocate memory block
void Mem_Free(void *ptr);