/***************************************************************************
                          realtemplateclasses.h  -  description
                             -------------------
    begin                : gio Apr 28 2005
    copyright            : (C) 2005 by Aurion s.r.l.
    email                : fabio.giovagnini@aurion-tech.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _REALTEMPLATECLASSES_H
#define _REALTEMPLATECLASSES_H


#ifdef _INSTANTIATE_TEMPLATE_
#include <q3cstring.h>
template class KaArray<char>;
template class KaArrayID<char>;
template class KaPArray< KaArray<char> >;
template class KaPArray< KaArrayID<char> >;
template class KaArray<unsigned int>;
template class KaArray<Q3CString>;
#endif

typedef KaArrayID<char> KwtddString;
typedef KaPArray< KaArrayID<char> > KwtddStringList;
typedef KaArray<unsigned int> Kdisplacement_t;


#endif
