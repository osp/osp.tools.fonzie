/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "magicmetrics.h"
#include "glyphtrace.h"

#include <QRectF>
#include <QDebug>
#include <QPainter>
#include <QImage>
#include <QTransform>
#include <QPair>
#include <cmath>
#include <QPainterPath>

//struct dd
//{
//	QPair<double,double> top;
//	QPair<double,double> bottom;
//};

MagicMetrics::MagicMetrics(QList<GlyphTrace*> gtl, QString ref, bool strictR)
	:hasReference(false),
	strictReference(strictR)
{
	//	QMap<GlyphTrace*, QPair<double,double> > mc;
	//	QMap<GlyphTrace*, dd > mc;
	if(!ref.isEmpty())
	{
		FT_Init_FreeType(&library);
		FT_New_Face(library, ref.toLocal8Bit(), 0, &face);
		hasReference = true;
	}
	Do(gtl);
	return;
	foreach(GlyphTrace* gt, gtl)
	{
		if(!gt->GetPath().isEmpty())
		{
			QTransform t;
			t.translate(gt->GetTx(), gt->GetTy());
			t.scale(gt->GetScale(), gt->GetScale());
			QPainterPath orig(gt->GetPath());
			QPainterPath p(t.map(orig));

			gt->SetAdvance(p.boundingRect().width() * 1.2);
			gt->SetTx(gt->GetTx()
				  -  p.boundingRect().left()
				  + (p.boundingRect().width() * 0.1));

			//			QTransform t2;
			//			t2.translate(gt->GetTx(), gt->GetTy());
			//			t2.scale(gt->GetScale(), gt->GetScale());
			//			QPainterPath p2(t2.map(orig));
			//			mc.insert(gt, QPair<double,double>(p2.boundingRect().left(), p2.boundingRect().width()));
		}
		else
		{
			gt->SetAdvance(462.0);
		}
	}

//	QMap<int, CPair> kitems;
	QList<CPair> pitems;
	QString cspace(" ");

	double spacingBase(20.0);

	foreach(GlyphTrace* base, gtl)
	{
		if(!base->GetPath().isEmpty())
		{
			qDebug()<<"Metrics:"<<base->GetName();
			QTransform t;
			t.translate(base->GetTx(), base->GetTy());
			t.scale(base->GetScale(), base->GetScale());
			QPainterPath pleft(t.map(base->GetPath()));
			const QRectF bbleft(pleft.boundingRect());


			foreach(GlyphTrace* other, gtl)
			{
				if(!other->GetPath().isEmpty())
				{
					QTransform t2;
					t2.translate(other->GetTx(), other->GetTy());
					t2.scale(other->GetScale(), other->GetScale());
					QPainterPath pright(t2.map(other->GetPath()));
					const QRectF bbright(pright.boundingRect());
//					double actualSpacing(base->GetAdvance() + bbright.left() - bbleft.right());

					//			if(actualSpacing > spacingBase)
					{
//						double baseCorrection(actualSpacing - spacingBase);

						// A
						double rrtop(qMin(bbleft.top(), bbright.top()));
						double rrbottom(qMax(bbleft.bottom(), bbright.bottom()));
//						qDebug()<<"RectLeft:"<<bbleft<<";\tRectRight:"<<bbright<<";\t"<<rrtop<<rrbottom;

						// pfff, go for rendering
						QImage wrImage(qRound(base->GetAdvance() + other->GetAdvance()),
							       qRound(rrbottom - rrtop),
							       QImage::Format_Mono);
						wrImage.fill(1);
						QPainter p(&wrImage);
						if(!p.isActive())
						{
							qDebug()<<"Failed to activate painter";
							qDebug()<<"\t"<<rrtop<<rrbottom;
							//					continue;
						}
						else
						{
							p.setBrush(Qt::black);
							p.setPen(Qt::NoPen);
							p.drawPath(pleft);
							p.translate(base->GetAdvance(), 0);
							p.drawPath(pright);
							int middle(base->GetAdvance());
							const int tw(wrImage.width());
							QList<double> vl;
							double vmin(999999.9);
							double vmax(-99999.9);
							double white(0.0);
							double black(0.0);
							for(int y(0); y < wrImage.height(); y += 10)
							{
								double I(0.0);
								bool Icontrol(true);
								for(int x(middle); x < tw; ++x)
								{
									if(qGray(wrImage.pixel(x,y)) == 0)
									{
										Icontrol = false;
										black += 1.0;
									}
									else
									{
										if(Icontrol)
											I += 1.0;
										white += 1.0;
									}
								}
								Icontrol = true;
								for(int x(tw - 1); x >= middle; --x)
								{
									if(qGray(wrImage.pixel(x,y)) == 0)
									{
										Icontrol = false;
										black += 1.0;
									}
									else
									{
										if(Icontrol)
											I += 1.0;
										white += 1.0;
									}
								}
								vmin = qMin(vmin,I);
								vmax = qMax(vmax,I);
								vl.append(I);
							}
							double vmean(Mean(vl));
							double stdvv(0.0);
							foreach(double v, vl)
							{
								stdvv += (v - vmean) * (v - vmean);
							}
							double stdv(sqrt(stdvv / double(vl.count())));

							vmin = (base->GetAdvance() + bbright.left()) - bbleft.right();
							double minwhite(white - (vmin * (rrbottom - rrtop)));
							double bwOrig(black * 100.0 / white);
							double bwCorr(black * 100.0 / minwhite);

//							int scorr( qRound( stdv * 0.12 ));
//							qDebug()<<"\t"<<other->GetName()<<vmean<<stdv<<"["<<vmin<<","<<vmax<<"] ["<< bwOrig <<","<<bwCorr<<"]";
							if((bwCorr - bwOrig) > (bwOrig / 20.0))
							{
								int scorr(qRound(qMin(pow(log(bwCorr - bwOrig ), 2.0), vmin)));
//								if(base->GetName() == other->GetName())
								qDebug()<<"\t"<<other->GetName()<<scorr;
//								pitems.insert(base->GetName(), QPair<QString,int>(other->GetName(), scorr));
								CPair pp;
								pp.left = base->GetName();
								pp.right = other->GetName();
								pp.val = scorr;
								pitems.append(pp);
//								if(kitems.contains(scorr))
//								{
//									if(!kitems[scorr].left.contains(base->GetName(true)))
//										kitems[scorr].left.append(base->GetName(true));
//									if(!kitems[scorr].right.contains(other->GetName(true)))
//										kitems[scorr].right.append(other->GetName(true));
//								}
//								else
//								{
//									CPair cp;
//									cp.left.append(base->GetName(true));
//									cp.right.append(other->GetName(true));
//									kitems.insert(scorr,cp);
//								}
							}
						}
					}

					//				// B
					//				QRectF TL(bbleft.left(), rrtop, bbleft.width(), rrheight);
					//				QRectF BL(bbleft.left(), rrtop - rrheight, bbleft.width(), rrheight);
					//				QRectF TR(bbright.left(), rrtop, bbright.width(), rrheight);
					//				QRectF BR(bbright.left(), rrtop - rrheight, bbright.width(), rrheight);
					//
					//				// C
					//				QPainterPath PTL;
					//				PTL.addRect(TL);
					//				QPainterPath PBL;
					//				PBL.addRect(BL);
					//				QPainterPath PTR;
					//				PTR.addRect(TR);
					//				QPainterPath PBR;
					//				PBR.addRect(BR);
					//
					//				// D
					//				QRectF ptl(pleft.intersected(PTL).boundingRect());
					//				QRectF pbl(pleft.intersected(PBL).boundingRect());
					//				QRectF ptr(pleft.intersected(PTR).boundingRect());
					//				QRectF pbr(pleft.intersected(PBR).boundingRect());
					//
					//				// E
					//				double dt(ptr.left() - ptl.right());
					//				double db(pbr.left() - pbl.right());
					//				QRectF baseRect;
					//				QRectF otherRect;
					//
					//				if((dt < db) && !ptl.isEmpty() && !ptr.isEmpty() )
					//				{
					//					baseRect = ptl;
					//					otherRect = ptr;
					//				}
					//				else if(!pbl.isEmpty() && !pbr.isEmpty())
					//				{
					//					baseRect = pbl;
					//					otherRect = pbr;
					//				}
					//				else
					//				{
					//					if(!ptl.isEmpty() && pbl.isEmpty())
					//					{
					//						baseRect = ptl;
					//						otherRect = pbr;
					//					}
					//					else
					//					{
					//						baseRect = ptl;
					//						otherRect = pbr;
					//					}
					//				}
					//
					//
					//				double cl(baseRect.left() + (baseRect.width() / 2.0));
					//				double cr(base->GetAdvance() + otherRect.left() + (otherRect.width() / 2.0));
					//				double v1(cr - cl);
					//				double el(baseRect.right());
					//				double er(base->GetAdvance() + otherRect.left());
					//				double v2(er - el);
					//
					//				double v3(((v1 + v2) / 2.0 ) / 100.0);
					//				int scorr( qRound( v3 * v3 ));
					//				if(double(scorr) > (v2 / 100.0))
					//				{
					//					if(kitems.contains(scorr))
					//					{
					//						if(!kitems[scorr].left.contains(base->GetName(true)))
					//							kitems[scorr].left.append(base->GetName(true));
					//						if(!kitems[scorr].right.contains(other->GetName(true)))
					//							kitems[scorr].right.append(other->GetName(true));
					//					}
					//					else
					//					{
					//						CPair cp;
					//						cp.left.append(base->GetName(true));
					//						cp.right.append(other->GetName(true));
					//						kitems.insert(scorr,cp);
					//					}
					//				}
				}
			}
		}
	}
	gposfeature = QString("\nfeature kern{\n");
	int c(0);
	bool lookupClosed(true);
	int lookupCounter(0);
	QString curLookup;
	foreach(CPair item, pitems)
	{
//		int ak(qAbs( key ));
//		QString l(kitems[key].left.join(cspace));
//		QString r(kitems[key].right.join(cspace));
		if(0 == c)
		{
			curLookup = QString("kern%1").arg(++lookupCounter);
			gposfeature += QString("\nlookup %1 useExtension {\n").arg(curLookup);
			lookupClosed = false;
		}
		gposfeature += QString("\tpos %1 %2 -%3;\n").arg(item.left).arg(item.right).arg(item.val);
		if(64 < c)
		{
			gposfeature += QString("} %1;\n").arg(curLookup);
			c = 0;
			lookupClosed = true;
		}
		else
		{
			c += 1;
		}
	}
	if(!lookupClosed)
		gposfeature += QString("} %1;\n").arg(curLookup);
	gposfeature += QString("\n}kern;");
	//		gt->DumpMetrics();
	//		if(false)
	//		{
	//			M m;
	//			QImage i;
	//			int w(i.width());
	//			int h(i.height());
	//			for(int y(0); y < h; ++y)
	//			{
	//				double dist(0.0);
	//				double scaledDist(0.0);
	//				for(int x(0); x < w; ++x)
	//				{
	//					if(qGray(i.pixel(x,y)) == 0)
	//						break;
	//					dist += 1.0;
	//				}
	//				//			scaledDist = (dist * double(w) / 1000.0);
	//				m.left.append(dist);
	//				m.minL = qMin(dist,m.minL);
	//				//			scaledDist = 0.0;
	//				dist = 0.0;
	//				for(int x(w - 1); x >= 0; --x)
	//				{
	//					if(qGray(i.pixel(x,y)) == 0)
	//						break;
	//					dist += 1.0;
	//				}
	//				m.right.append(dist);
	//				m.minR = qMin(dist, m.minR);
	//			}
	//			metrics.insert(gt, m);
	//		}
	//	}
	//
	//	if(false)
	//	{
	//		gposfeature = QString("\nfeature kern{\n");
	//		Do();
	//		gposfeature += QString("\n}kern;");
	//	}
}

double MagicMetrics::Mean(const QList<double> &l)
{
	double ret(0.0);
	foreach(double v, l)
	{
		ret += v;
	}
	return ret / double(l.count());
}

double MagicMetrics::Min(const QList<double> &l)
{
	double ret(99999.0);
	foreach(double v, l)
	{
		ret = qMin(ret, v);
	}
	return ret;
}


void MagicMetrics::Do(QList<GlyphTrace*> gtl)
{
	if(hasReference)
	{
		foreach(GlyphTrace * base, gtl)
		{
			double scale(base->GetScale());
			uint glyphIndex  = FT_Get_Char_Index ( face, base->GetCharcode() );
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_SCALE);
			double xadvance = face->glyph->metrics.horiAdvance;
			double origLeft = face->glyph->metrics.horiBearingX;
			double origWidth = face->glyph->metrics.width;
			{
				QPainterPath p(base->GetPath());
				QRectF bbox(p.boundingRect());
				double bboxW(bbox.width());
				double left(bbox.left());
				if(strictReference)
				{
					base->SetAdvance( xadvance );
					if(QChar::category(base->GetCharcode()) != QChar::Separator_Space)
						base->SetTx((-left * scale) + (origLeft));
				}
				else
				{
					if(QChar::category(base->GetCharcode()) != QChar::Separator_Space)
					{
						double fact((bboxW * scale) / origWidth);
						base->SetAdvance( xadvance * fact);
						base->SetTx((-left * scale) + (origLeft * fact));
					}
					else
						base->SetAdvance( xadvance);
				}
//				qDebug()<<"Metrics" <<QChar(base->GetCharcode()) <<xadvance <<scale <<(xadvance - ((origLeft + bboxW) * scale)) ;
			}
		}

	}
	else
	{
		foreach(GlyphTrace * base, gtl)
		{
			bool isSpace(QChar::category(base->GetCharcode()) == QChar::Separator_Space);
			if(isSpace)
				base->SetAdvance(400.0);
			else
			{
				QPainterPath p(base->GetPath());
				QRectF bbox(p.boundingRect());
				double bboxW(bbox.width());
				double left(bbox.left());
				double bonus(bboxW * (1 /log( bboxW * bboxW )));
				base->SetAdvance( (2 * bonus *  base->GetScale()) + (bboxW * base->GetScale()) );
				p.translate(bonus - left,0);
				base->setPath(p);
			}
		}
	}
}
