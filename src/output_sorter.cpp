/*
 * This File is part of Pindel; a program to locate genomic variation.
 * https://trac.nbic.nl/pindel/
 *
 *   Copyright (C) 2011 Kai Ye
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// System header files
#include <iostream>
#include <fstream>

// Pindel header files
#include "pindel.h"
#include "logdef.h"
#include "output_sorter.h"

OutputSorter::OutputSorter (const unsigned &NumBoxes_in, const std::string & CurrentChr_in,
                            std::ofstream & InvOutf_in)
{
   NumBoxes = NumBoxes_in;
   CurrentChr = new std::string (CurrentChr_in);
   InvOutf = &InvOutf_in;
}

OutputSorter::~OutputSorter ()
{
   delete CurrentChr;
}

void OutputSorter::SortAndOutputInversions (std::vector < SPLIT_READ > &Reads, std::vector<unsigned> Inv[])
{
   LOG_INFO (std::cout << "Sorting and outputing Inversions ..." << std::endl);
   DoSortAndOutputInversions(Reads, Inv, false);
   LOG_INFO (std::cout << "Inversions (INV): " << g_numberOfInvInstances << std::endl << std::endl);
}

void
OutputSorter::SortAndOutputNonTemplateInversions (std::vector<SPLIT_READ>& Reads, std::vector<unsigned> Inv[])
{
   LOG_INFO (std::cout << "Sorting and outputing Inversions with non-template sequence ..." <<
             std::endl);
   int Count_INV_NT_output = DoSortAndOutputInversions(Reads, Inv, true);

   LOG_INFO (std::cout << "Inversions with non-template sequence (INV_NT): " <<
             Count_INV_NT_output << std::endl << std::endl);
}

int
OutputSorter::DoSortAndOutputInversions (std::vector<SPLIT_READ> &Reads,
      std::vector<unsigned> Inv[],
      bool isNonTemplateInversion)
{
    //std::cout << "start" << std::endl;
   unsigned int InversionsNum;
   short CompareResult;
   unsigned Temp4Exchange;
   unsigned int GoodNum;
   std::vector<SPLIT_READ> GoodIndels;
   std::vector<Indel4output> IndelEvents;
   int ReportedEventCount = 0;
    //std::cout << "start1" << std::endl;
   for (unsigned Box_index = 0; Box_index < NumBoxes; Box_index++) {
      if (Inv[Box_index].size () >= NumRead2ReportCutOff) {
         InversionsNum = Inv[Box_index].size ();
         LOG_DEBUG (std::cout << Box_index << "\t" << Inv[Box_index].size () << std::endl);
          //std::cout << Box_index << std::endl;
         for (unsigned int First = 0; First < InversionsNum - 1; First++) {
            for (unsigned int Second = First + 1; Second < InversionsNum;
                  Second++) {
               LOG_DEBUG (std::cout << InputIndels[First].BPLeft << "\t" << InputIndels[First].BPRight << "\t"
                          InputIndels[Second].BPLeft << "\t" << InputIndels[Second].BPRight << std::endl);
               {
                  if (Reads[Inv[Box_index][First]].ReadLength == Reads[Inv[Box_index][Second]].ReadLength) {
                     if (Reads[Inv[Box_index][First]].LeftMostPos == Reads[Inv[Box_index][Second]].LeftMostPos) {
                        Reads[Inv[Box_index][Second]].UniqueRead = false;
                     }
                  }
                  if (Reads[Inv[Box_index][First]].BPLeft + Reads[Inv[Box_index][First]].BPRight < Reads[Inv[Box_index][Second]].BPLeft + Reads[Inv[Box_index][Second]].BPRight) {
                     continue;
                  }
                  else if (Reads[Inv[Box_index][First]].BPLeft + Reads[Inv[Box_index][First]].BPRight > Reads[Inv[Box_index][Second]].BPLeft + Reads[Inv[Box_index][Second]].BPRight) {
                     CompareResult = 1;
                  }
                  else if (Reads[Inv[Box_index][First]].IndelSize > Reads[Inv[Box_index][Second]].IndelSize) { // IndelSize: larger ones first
                     continue;
                  }
                  else if (Reads[Inv[Box_index][First]].IndelSize < Reads[Inv[Box_index][Second]].IndelSize) {
                     CompareResult = 1;
                  }
                  else if (Reads[Inv[Box_index][First]].BPLeft < Reads[Inv[Box_index][Second]].BPLeft) {
                     continue;
                  }
                  else if (Reads[Inv[Box_index][First]].BPLeft > Reads[Inv[Box_index][Second]].BPLeft) {
                     CompareResult = 1;
                  }
                  else if (Reads[Inv[Box_index][First]].BPLeft ==
                           Reads[Inv[Box_index][Second]].BPLeft) {
                     if (Reads[Inv[Box_index][First]].BPRight <
                           Reads[Inv[Box_index][Second]].BPRight) {
                        continue;
                     }
                     else if (Reads[Inv[Box_index][First]].BPRight >
                              Reads[Inv[Box_index][Second]].BPRight) {
                        CompareResult = 1;
                     }
                     else if (isNonTemplateInversion) {
                        if (Reads[Inv[Box_index][First]].NT_size <
                              Reads[Inv[Box_index][Second]].NT_size) {
                           continue;
                        }
                        else if (Reads[Inv[Box_index][First]].NT_size >
                                 Reads[Inv[Box_index][Second]].NT_size) {
                           CompareResult = 1;
                        }
                     }
                  }
                  if (CompareResult == 1) {
                     Temp4Exchange = Inv[Box_index][First];
                     Inv[Box_index][First] = Inv[Box_index][Second];
                     Inv[Box_index][Second] = Temp4Exchange;
                  }
               }
            }
         }
          //std::cout << "in1" << std::endl;
         GoodIndels.clear ();
         IndelEvents.clear ();
          //std::cout << "in2" << std::endl;
         for (unsigned int First = 0; First < InversionsNum; First++) {
            GoodIndels.push_back (Reads[Inv[Box_index][First]]);
         }
          //std::cout << "in3" << std::endl;
         GoodNum = GoodIndels.size ();
         LOG_DEBUG (std::cout << "GoodNum " << Box_index << " " << GoodNum << std::endl);
         if (GoodNum == 0) {
            continue;
         }
         LOG_DEBUG (std::cout << GoodNum << std::endl);
         Indel4output OneIndelEvent;
         OneIndelEvent.Start = 0;
         OneIndelEvent.End = 0;
         OneIndelEvent.Support = OneIndelEvent.End - OneIndelEvent.Start + 1;
         OneIndelEvent.BPLeft = GoodIndels[0].BPLeft;
         OneIndelEvent.BPRight = GoodIndels[0].BPRight;
         OneIndelEvent.WhetherReport = true;
          //std::cout << "in4" << std::endl;
         for (unsigned int GoodIndex = 1; GoodIndex < GoodNum; GoodIndex++) {
            LOG_DEBUG (std::cout << GoodIndex <<
                       "\t" << GoodIndels[GoodIndex].BPLeft <<
                       "\t" << GoodIndels[GoodIndex].BPRight <<
                       "\t" << OneIndelEvent.BPLeft <<
                       "\t" << OneIndelEvent.BPRight << std::endl);
            if (GoodIndels[GoodIndex].BPLeft + GoodIndels[GoodIndex].BPRight == OneIndelEvent.BPLeft + OneIndelEvent.BPRight) {
               OneIndelEvent.End = GoodIndex;
            }
            else {
               // change breakpoints
               // step 1 find the largest event
               unsigned MaxSize = 0;
               for (unsigned i = OneIndelEvent.Start; i <= OneIndelEvent.End; i++) {
                  if (GoodIndels[i].IndelSize > MaxSize) {
                     MaxSize = GoodIndels[i].IndelSize;
                  }
               }
                //std::cout << "MaxSize " << MaxSize << std::endl;
               // changed BP according to the largest event
               for (unsigned i = OneIndelEvent.Start; i <= OneIndelEvent.End; i++) {
                  if (GoodIndels[i].IndelSize / (float)MaxSize < 0.95 || MaxSize  + 30 > GoodIndels[i].ReadLength + GoodIndels[i].IndelSize) {
                     OneIndelEvent.WhetherReport = false;
                     //std::cout << "Skip one inversion 1!" << std::endl;
                     break;
                  }
                  // if (MaxSize < GoodIndels[i].IndelSize)
                  //std::cout << "MaxSize " << MaxSize << " > " << GoodIndels[i].IndelSize << std::endl; 
                  short Diff = (MaxSize - GoodIndels[i].IndelSize) / 2;
                  GoodIndels[i].IndelSize = MaxSize;
                  GoodIndels[i].BPLeft = GoodIndels[i].BPLeft - Diff;
                  GoodIndels[i].BPRight = GoodIndels[i].BPRight + Diff;
                   
                  // for plus
                  if (GoodIndels[i].MatchedD == '+') {
                      //std::cout << GoodIndels[i].BP << " > " << Diff << std::endl;
                      if (GoodIndels[i].BP > Diff) {
                          //std::cout << "+ " << GoodIndels[i].BP << " " << Diff << std::endl;
                          GoodIndels[i].BP = GoodIndels[i].BP - Diff;
                      }
                  }
                  else {
                       if (GoodIndels[i].BP + Diff < GoodIndels[i].ReadLengthMinus)
                     GoodIndels[i].BP = GoodIndels[i].BP + Diff;   // for minus
                  }
                   //if (GoodIndels[i].BP < 0) std::cout << "here " << Diff << " " << GoodIndels[i].BP << std::endl;
               }
               OneIndelEvent.RealStart = GoodIndels[OneIndelEvent.Start].BPLeft; // largest one
               OneIndelEvent.RealEnd = GoodIndels[OneIndelEvent.Start].BPRight;  // largest one
               OneIndelEvent.Support =
                  OneIndelEvent.End - OneIndelEvent.Start + 1;
               if (OneIndelEvent.WhetherReport) {
                  IndelEvents.push_back (OneIndelEvent);
               }
               OneIndelEvent.Start = GoodIndex;
               OneIndelEvent.End = GoodIndex;
               OneIndelEvent.BPLeft = GoodIndels[GoodIndex].BPLeft;
               OneIndelEvent.BPRight = GoodIndels[GoodIndex].BPRight;
            }
         }
          //std::cout << "in5" << std::endl;
         // last element
         // change breakpoints
         // step 1 find the largest event
         unsigned MaxSize = 0;
         for (unsigned i = OneIndelEvent.Start; i <= OneIndelEvent.End; i++) {
            if (GoodIndels[i].IndelSize > MaxSize) {
               MaxSize = GoodIndels[i].IndelSize;
            }
         }
          //std::cout << "in6" << std::endl;
         // changed BP according to the largest event
         for (unsigned i = OneIndelEvent.Start; i <= OneIndelEvent.End; i++) {
            if (GoodIndels[i].IndelSize / (float)MaxSize < 0.95 || MaxSize  + 30 > GoodIndels[i].ReadLength + GoodIndels[i].IndelSize) {
               OneIndelEvent.WhetherReport = false;
               //std::cout << "Skip one inversion 2!" << std::endl;
               break;
            }
            short Diff = (MaxSize - GoodIndels[i].IndelSize) / 2;
            GoodIndels[i].IndelSize = MaxSize;
            GoodIndels[i].BPLeft = GoodIndels[i].BPLeft - Diff;
            GoodIndels[i].BPRight = GoodIndels[i].BPRight + Diff;
            // for plus
            if (GoodIndels[i].MatchedD == '+') {
               if (GoodIndels[i].BP > Diff)  
                   GoodIndels[i].BP = GoodIndels[i].BP - Diff;
            }
            else {
                if (GoodIndels[i].BP + Diff < GoodIndels[i].ReadLengthMinus)
                   GoodIndels[i].BP = GoodIndels[i].BP + Diff;   // for minus
            }
            //if (GoodIndels[i].BP < 0) std::cout << "there " << Diff << " " << GoodIndels[i].BP << std::endl;
         }
         // std::cout << "in7" << std::endl;
         OneIndelEvent.RealStart = OneIndelEvent.BPLeft;
         OneIndelEvent.RealEnd = OneIndelEvent.BPRight;
         OneIndelEvent.Support = OneIndelEvent.End - OneIndelEvent.Start + 1;
         LOG_DEBUG (std::cout << OneIndelEvent.Support
                    << "\t" << OneIndelEvent.Start
                    << "\t" << OneIndelEvent.End << std::endl);
         if (OneIndelEvent.WhetherReport) {
            IndelEvents.push_back (OneIndelEvent);
         }
          //std::cout << "in8" << std::endl;
         LOG_DEBUG (std::cout << "IndelEvent: " << IndelEvents.size () << std::endl);

         if (IndelEvents.size ()) {
            ReportedEventCount += ReportIndelEvents (IndelEvents, GoodIndels);
         }
          //std::cout << "in9" << std::endl;
      }
   }
    //std::cout << "end" << std::endl;
   return ReportedEventCount;
}

int OutputSorter::ReportIndelEvents (std::vector<Indel4output> &IndelEvents,
                                 std::vector<SPLIT_READ> &GoodIndels)
{  
    //std::cout << "ReportIndelEvents" << std::endl;
   int ReportedEventCount = 0;
   for (unsigned EventIndex = 0; EventIndex < IndelEvents.size ();
         EventIndex++) {
       //std::cout << EventIndex << std::endl;
      LOG_DEBUG (std::cout << IndelEvents[EventIndex].Start <<
                 "\t" << IndelEvents[EventIndex].End <<
                 "\t" << IndelEvents[EventIndex].Support << std::endl);
      unsigned int RealStart = IndelEvents[EventIndex].RealStart;
      unsigned int RealEnd = IndelEvents[EventIndex].RealEnd;
      if (IndelEvents[EventIndex].Support < NumRead2ReportCutOff) {
         continue;
      }
      // std::cout << "ReportIndelEvents 1" << std::endl;
      if (GoodIndels[IndelEvents[EventIndex].Start].IndelSize <
            BalanceCutoff) {
          //std::cout << "ReportIndelEvents 2" << std::endl;
         OutputInversions (GoodIndels, *CurrentChr,
                           IndelEvents[EventIndex].Start,
                           IndelEvents[EventIndex].End,
                           RealStart, RealEnd, *InvOutf);
         // std::cout << "ReportIndelEvents 2'" << std::endl;
         ReportedEventCount++;
      }
      else if (ReportEvent
               (GoodIndels, IndelEvents[EventIndex].Start,
                IndelEvents[EventIndex].End)) {
                  // std::cout << "ReportIndelEvents 3" << std::endl;
         OutputInversions (GoodIndels, *CurrentChr,
                           IndelEvents[EventIndex].Start,
                           IndelEvents[EventIndex].End,
                           RealStart, RealEnd, *InvOutf);
                  // std::cout << "ReportIndelEvents 3'" << std::endl;
         ReportedEventCount++;
      }
   }
   return ReportedEventCount;
}
