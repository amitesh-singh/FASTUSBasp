/*
 * FASTUSBasp v2 - Fastest programmer for AVR
 * Copyright (C) 2018  Amitesh Singh <singh.amitesh@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _STM32_QUEUE_H
#define _STM32_QUEUE_H

namespace util
{
   template<class T>
      class queue
        {
         protected:
            int _front, _back, _count;
            T *_data;
            int _maxitems;
         public:
            queue(int maxitems, T *buf)
              {
                 _front = 0;
                 _back = 0;
                 _count = 0;
                 _maxitems = maxitems;
                 // don't use this since it increases the size of binary by 48k on stm32 :/
                 //_data = new T[maxitems + 1];
                 _data = buf;
              }

            ~queue()
              {
                 //delete[] _data;
              }

            inline int size()
              {
                 return _count;
              }

            void push(const T &item);

            T &front()
              {
                 return _data[_front];
              }

            T &back()
            {
                return _data[_back - 1];
            }

            T &pop();

            inline void clear()
              {
                 _front = _back = 0;
                 _count = 0;
              }

            inline bool empty()
              {
                 if (_count <= 0)
                   return true;
                 else return false;
              }
        };

   template<class T>
      void queue<T>::push(const T &item)
        {
           if(_count < _maxitems)
             { // Drops out when full
                _data[_back++]=item;
                ++_count;
                // Check wrap around
                if (_back >= _maxitems)
                  _back = 0;
             }
        }

   template<class T>
      T &queue<T>::pop()
        {
           _front++;
           --_count;
           // Check wrap around
           if (_front >= _maxitems)
             _front = 0;
           return _data[_front];

        }
}

#endif
