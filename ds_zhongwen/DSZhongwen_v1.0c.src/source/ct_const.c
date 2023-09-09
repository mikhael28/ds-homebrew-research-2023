/*
Copyright (c) 2007 Rodrigo Roman and Ramon Roman (rrc2soft)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of "rrc2soft" nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Includes
#include <PA9.h>       // Include for PA_Lib

#include "ct_const.h"

char *ct_const_getText(int id, int language)
{
   switch (language) {
      case LANG_ENG:
         switch (id) {
            case TXT_INI_ENGLISH : return "English";
            case TXT_INI_SPANISH : return "Español";
            case TXT_INI_PROFILE : return "---";
            case TXT_MADEBY_ : return "DSZHONGWEN";
            case TXT_CFG_ : return "CONFIGURATION";            
            case TXT_CFG_SPEED : return "Speed";
            case TXT_CFG_HAND : return "Hand";
            case TXT_CFG_LHAND : return "Left-Handed";
            case TXT_CFG_RHAND : return "Right-Handed";
            case TXT_MM_ : return "MAIN MENU";
            case TXT_MM_EXAMS: return "Exams";
            case TXT_MM_GRADES: return "Grades";
            case TXT_MM_IDEOGRAMS : return "Ideograms";
            case TXT_MM_LESSON : return "Lesson";
            case TXT_GETLESS_ : return "PICK UP A LESSON";
            case TXT_GETLESS_GRAMMAR : return "Grammar";
            case TXT_GETLESS_IDEOGRAM : return "Ideogram";
            case TXT_GETLESS_LISTEN : return "Listening";
            case TXT_GETLESS_NIHAO : return "General";
            case TXT_GETLESS_HANZIATTACK : return "HanziAttack!";
            case TXT_REVIEWHANZI_ : return "SELECT AN IDEOGRAM";
            case TXT_LESSON_ : return "LESSON";
            case TXT_LESSON_GO : return "TOUCH ME TO \n START THE LESSON";
            case TXT_LESSON_NO : return "TOUCH ME TO \n RETURN TO THE MAIN SCREEN";
            case TXT_LESSON_END : return "Lesson finished!";            
            case TXT_LESSON_GRAM_ : return "GRAMMAR";
            case TXT_LESSON_HANZI_ : return "IDEOGRAM";
            case TXT_LESSON_LISTEN_ : return "LISTENING";                        
            case TXT_EXAM_ : return "EXAMS";                        
            case TXT_EXAM_QUIZ : return "Quiz";            
				case TXT_EXAM_CARDS : return "Cards";            
				case TXT_EXAM_LISTEN : return "Listen";            
				case TXT_EXAM_MEANING : return "Meaning?";
            case TXT_GRADES_ : return "GRADES";               				
				case TXT_DICT_ : return "DICTIONARY";               				            
				case TXT_DICT_TEXT : return "Tool - Dictionary";               				
            return "";
      	}   
      case LANG_SPA:
         switch (id) {
            case TXT_INI_ENGLISH : return "English";
            case TXT_INI_SPANISH : return "Español";
            case TXT_INI_PROFILE : return "---";
            case TXT_MADEBY_ : return "DSZHONGWEN";
            case TXT_CFG_ : return "CONFIGURACION";
            case TXT_CFG_SPEED : return "Velocidad";
            case TXT_CFG_HAND : return "Mano";
            case TXT_CFG_LHAND : return "Zurdo";
            case TXT_CFG_RHAND : return "Diestro";            
				case TXT_MM_ : return "MENU PRINCIPAL";
            case TXT_MM_EXAMS: return "Examenes";
            case TXT_MM_GRADES: return "Notas";
            case TXT_MM_IDEOGRAMS: return "Ideogramas";
            case TXT_MM_LESSON : return "Leccion";
            case TXT_GETLESS_ : return "ELIGE UNA LECCION";
            case TXT_GETLESS_HANZIATTACK : return "HanziAttack!";            
            case TXT_GETLESS_GRAMMAR : return "Gramatica";
            case TXT_GETLESS_IDEOGRAM : return "Ideogramas";
            case TXT_GETLESS_LISTEN : return "Conversacion";
            case TXT_GETLESS_NIHAO : return "General"; 
				case TXT_REVIEWHANZI_ : return "ELIGE UN IDEOGRAMA";           
				case TXT_LESSON_ : return "LECCION";
            case TXT_LESSON_GO : return "TOCA AQUI PARA \n COMENZAR LA LECCION";
            case TXT_LESSON_NO : return "TOCA AQUI PARA \n VOLVER AL MENU PRINCIPAL";				
            case TXT_LESSON_END : return "Leccion finalizada!";
            case TXT_LESSON_GRAM_ : return "GRAMATICA";
            case TXT_LESSON_HANZI_ : return "IDEOGRAMAS";
            case TXT_LESSON_LISTEN_ : return "CONVERSACION";                                    
				case TXT_EXAM_ : return "EXAMENES";                                    
            case TXT_EXAM_QUIZ : return "Test";            
				case TXT_EXAM_CARDS : return "Recordar";            
				case TXT_EXAM_LISTEN : return "Escuchar";             
				case TXT_EXAM_MEANING : return "Significado?";
            case TXT_GRADES_ : return "NOTAS";				
				case TXT_DICT_ : return "DICCIONARIO";               				            
				case TXT_DICT_TEXT : return "Herr. - Diccionario";               				
            return "";
      	}            
      return "";
	}   
	return "";
}
