/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ChartRawReportProcessor.java,v $
 * $Revision: 1.3 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package com.sun.star.report.pentaho.output.chart;

import com.sun.star.report.DataSourceFactory;
import com.sun.star.report.ImageService;
import com.sun.star.report.InputRepository;
import com.sun.star.report.OutputRepository;
import org.jfree.report.DataSourceException;
import org.jfree.report.ReportDataFactoryException;
import org.jfree.report.ReportProcessingException;
import org.jfree.report.flow.ReportJob;
import org.jfree.report.flow.ReportStructureRoot;
import org.jfree.report.flow.ReportTarget;
import org.jfree.report.flow.SinglePassReportProcessor;
import org.jfree.resourceloader.ResourceManager;

/**
 *
 * @author Ocke Janssen
 */
public class ChartRawReportProcessor extends SinglePassReportProcessor
{

    private OutputRepository outputRepository;
    private String targetName;
    private InputRepository inputRepository;
    private ImageService imageService;
    private DataSourceFactory dataSourceFactory;

    public ChartRawReportProcessor(final InputRepository inputRepository,
            final OutputRepository outputRepository,
            final String targetName,
            final ImageService imageService,
            final DataSourceFactory dataSourceFactory)
    {
        if (inputRepository == null)
        {
            throw new NullPointerException();
        }
        if (outputRepository == null)
        {
            throw new NullPointerException();
        }
        if (targetName == null)
        {
            throw new NullPointerException();
        }
        if (imageService == null)
        {
            throw new NullPointerException();
        }
        if (dataSourceFactory == null)
        {
            throw new NullPointerException();
        }
        this.targetName = targetName;
        this.inputRepository = inputRepository;
        this.outputRepository = outputRepository;
        this.imageService = imageService;
        this.dataSourceFactory = dataSourceFactory;
    }

    protected ReportTarget createReportTarget(final ReportJob job)
            throws ReportProcessingException
    {
        final ReportStructureRoot report = job.getReportStructureRoot();
        final ResourceManager resourceManager = report.getResourceManager();

        return new ChartRawReportTarget(job, resourceManager, report.getBaseResource(),
                inputRepository, outputRepository, targetName, imageService, dataSourceFactory);
    }

    public void processReport(final ReportJob job) throws ReportDataFactoryException, DataSourceException,
            ReportProcessingException
    {
        final ReportTarget reportTarget = createReportTarget(job);
        // first run: collect table cell sizes for all tables
        processReportRun(job, reportTarget);
        // second run: uses table cell data to output a single uniform table
        processReportRun(job, reportTarget);
    }
}
